#include "lsm.c"
#include <errno.h>  // Corregido: En C puro es errno.h, no cerrno
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int SUCCESS = 0;

LSM* module_manager;

enum Role {
    USER,
    ADMIN,
};

typedef struct {
    int role;
} task_sec;

typedef struct {
    int hidden;
} file_sec;

// 1. EL HOOK: Corregimos la firma para que coincida exactamente con open_hook_fn
int sec_file_check(task_struct* task, inode* node, int mask) {
    task_sec* ts = (task_sec*)task->security;
    file_sec* fs = (file_sec*)node->security;

    printf("  [LSM Hook] Evaluando acceso a archivo...\n");

    // Si el archivo está oculto y el usuario no es admin (o no tiene rol), denegar.
    if (fs && fs->hidden && (!ts || ts->role != ADMIN)) {
        printf("  [LSM Hook] -> DENEGADO: El archivo esta oculto y requiere rol ADMIN.\n");
        return EACCES;
    }

    printf("  [LSM Hook] -> CONCEDIDO: Pasa las politicas de seguridad.\n");
    return SUCCESS;
}

// 2. EL KERNEL: Emulamos la llamada al sistema real
int sys_open(task_struct* current_task, inode* current_inode, int mask) {
    printf("\n--- EJECUTANDO open() SYSCALL ---\n");
    printf("  [Kernel] Comprobaciones iniciales del sistema completadas.\n");

    // Iteramos por todos los hooks registrados en el framework LSM
    for (unsigned int i = 0; i < module_manager->active_hooks; i++) {
        int res = module_manager->hooks[i].fun(current_task, current_inode, mask);

        if (res != SUCCESS) {
            printf("[RESULTADO FINAL] ERROR: Acceso bloqueado por el framework (Codigo %d).\n", res);
            return res; // El acceso falla inmediatamente si un hook lo rechaza
        }
    }

    printf("[RESULTADO FINAL] EXITO: Archivo abierto correctamente.\n");
    return SUCCESS;
}

// Para limpiar el buffer del teclado en la consola interactiva
void flush_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    // Inicializamos el framework LSM global
    module_manager = (LSM*)malloc(sizeof(LSM));
    module_manager->active_hooks = 0;
    module_manager->hooks = NULL;

    // Registramos nuestro módulo de seguridad (sec_file_check)
    hook my_security_module;
    init_hook(&my_security_module, 1, sec_file_check);
    register_module(module_manager, &my_security_module);

    printf("[Boot] Framework LSM inicializado con %d modulo(s).\n", module_manager->active_hooks);

    // Creamos estructuras de prueba
    task_struct user_task;
    task_sec u_sec = { .role = USER };
    user_task.security = &u_sec;

    inode test_file;
    file_sec f_sec = { .hidden = 0 };
    test_file.security = &f_sec;

    int opcion;
    while (1) {
        printf("\n=========================================\n");
        printf(" SIMULADOR LSM INTEGRADO \n");
        printf("=========================================\n");
        printf("ESTADO ACTUAL:\n");
        printf(" -> Usuario: Rol [%s]\n", u_sec.role == ADMIN ? "ADMIN" : "USER");
        printf(" -> Archivo: Visibilidad [%s]\n", f_sec.hidden ? "OCULTO (Requiere Admin)" : "PUBLICO");

        printf("\nOPCIONES:\n");
        printf("1. Cambiar Rol del Usuario (0: USER, 1: ADMIN)\n");
        printf("2. Cambiar Atributo del Archivo (0: PUBLICO, 1: OCULTO)\n");
        printf("3. Ejecutar Syscall sys_open()\n");
        printf("4. Salir\n");
        printf("Elige una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            flush_stdin();
            continue;
        }

        switch (opcion) {
            case 1:
                printf("\nNuevo rol (0 para USER, 1 para ADMIN): ");
                scanf("%d", &u_sec.role);
                break;
            case 2:
                printf("\nNuevo atributo (0 para PUBLICO, 1 para OCULTO): ");
                scanf("%d", &f_sec.hidden);
                break;
            case 3:
                sys_open(&user_task, &test_file, 0);
                break;
            case 4:
                // Liberación de memoria para evitar leaks
                free(module_manager->hooks);
                free(module_manager);
                return 0;
            default:
                printf("Opcion invalida.\n");
        }
    }
    return 0;
}