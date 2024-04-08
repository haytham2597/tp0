#include "client.h"
#include "commons/string.h"
#include "commons/collections/dictionary.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;
	/*char *current_dir = getcwd(NULL, 0)+"/cliente.config";
	
	printf("%s\n", current_dir);*/


	/* ---------------- LOGGING ---------------- */
	//logger = log_create("logClient.log", "Cliente", 1, LOG_LEVEL_DEBUG);
	logger = iniciar_logger();
	log_info(logger, "Hola log de mierda");
	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */
	config = iniciar_config();
	valor = config_get_string_value(config, "CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	printf("%s\n", ip);
	printf("%s\n", puerto);
	
	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	// Loggeamos el valor de config


	/* ---------------- LEER DE CONSOLA ---------------- */

	//leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	//conexion = crear_conexion("127.0.0.1", "4444");
	log_info(logger, "Conexion %d", conexion);
	// Enviamos al servidor el valor de CLAVE como mensaje
	while(1){
		char* leido = readline("> ");
		if(string_equals_ignore_case(leido, "close")){
			t_paquete* packet = malloc(sizeof(t_paquete));
			packet->codigo_operacion=-1;
			enviar_paquete(packet, conexion);
			free(packet);
			break;
		}
		enviar_mensaje(leido, conexion);
	}

	// Armamos y enviamos el paquete
	//paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = malloc(sizeof(t_log));
	
	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = malloc(sizeof(t_config));
	char* pathname = "cliente.config";
	nuevo_config->path = malloc(sizeof(char*));
	strcpy(nuevo_config->path, pathname);
	//memcpy(nuevo_config->path, &pathname, strlen(pathname))
	nuevo_config->properties = dictionary_create();
	//config_save(nuevo_config);
	FILE* file = fopen(pathname, "rb");
	if(file == NULL)
	{
		printf("No pudo abrir el archivo");
		exit(EXIT_FAILURE);
	}
	ssize_t read;
	size_t len=0;
	char* line = NULL;
	while ((read = getline(&line, &len, file)) != -1){
		char** spl = string_split(line,"=");
        char* key = *(spl+0);
		char* value = *(spl+1);
		size_t ln = strlen(value) - 1; 
		if (*value && value[ln] == '\n') //El ultimo valor contiene una nueva línea
			value[ln] = '\0';
		
		if(string_is_empty(key) || string_is_empty(value))
			continue;
		config_set_value(nuevo_config, key, value);
	}
	fclose(file);
	if(line)
		free(line);
	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	// La primera te la dejo de yapa
	char* leido = readline("> ");
	log_info(logger, leido);
	if(leido)
		free(leido);
	// El resto, las vamos leyendo y logueando hasta recibir un string vacío


	// ¡No te olvides de liberar las lineas antes de regresar!

}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido = readline(">");
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = 1;
	paquete->buffer = malloc(sizeof(t_buffer));

	agregar_a_paquete(paquete, leido, strlen(leido));
	// Leemos y esta vez agregamos las lineas al paquete
	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	enviar_paquete(paquete, conexion);
	free(paquete->buffer);
	free(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	if(conexion > 0)
		close(conexion);
	if(logger)
		free(logger);
	if(config)
		free(config);
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
}