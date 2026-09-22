Operativos-Parte-uno

Descripción

En este proyecto se está simulando cómo el framework LSM (Linux Security Modules) logra implementar módulos de seguridad en el kernel sin alterar su funcionamiento base.

Para ello, implementamos varias estructuras clave, como los inodos y las tareas, cada una con sus propias reglas. Al manejar la seguridad mediante punteros genéricos, logramos que las verificaciones se realicen de forma abstracta, sin importar la regla específica que impongamos.

El diseño del sistema se articula de la siguiente manera:

Sistema de Hooks: Está diseñado para realizar las verificaciones necesarias al implementar nuestros módulos. Define una estructura de hook que vincula las tareas (tasks) con los inodos, asignándoles además un nivel de prioridad.

Gestor Core (LSM): Es el archivo principal donde se cargan los módulos y se almacenan todos los hooks registrados. Maneja la reasignación dinámica de memoria (resize) para poder añadir módulos de forma cómoda.

Integración (main): Es el punto donde se unen todas las estructuras. Aquí definimos una regla de seguridad que cargaremos como módulo: esta verifica si el archivo tiene el atributo de "oculto" y, si el usuario no es administrador, rechaza el acceso.

Además, emulamos la estructura de la llamada al sistema para abrir un archivo. Esta función, básicamente, se encarga de ejecutar todos los hooks que estén definidos en nuestro gestor de módulos (en este caso, solo tenemos el que verifica el rol de admin). Finalmente, inicializamos los valores y desplegamos una consola interactiva. Esto nos permite simular de forma escalable cómo se corren todas las configuraciones de seguridad sin necesidad de modificar el código fuente de la función original que abre los archivos.

Ejecución

Solo es necesario correr el archivo main y ya tendrás acceso a la consola interactiva para poder probar y jugar con las consideraciones de seguridad.

El sistema es escalable: en caso de querer añadir una nueva regla, se puede definir una nueva función en el main y pasarla mediante la función de añadir hook. De esta manera, el gestor de módulos hará automáticamente las verificaciones correspondientes a la regla propuesta.
