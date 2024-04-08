#include "server.h"
#include "commons/string.h"
int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);
	log_info(logger, "Cliente FD %d", cliente_fd);
	
	t_list* lista;
	while (1) {
		/*recibir_mensaje(cliente_fd);
		log_info(logger, "Recibi el mensaje");
		char* content="Te comento que recibi el mensaje";
		int s = send(cliente_fd, &content, string_length(content), MSG_WAITALL);
		if(s < 0)
		{
			printf("Nse pudo enviar mensaje me voy a la mierda\n");	
		}
		printf("Send server: %d\n", s);*/
		
		int cod_op = recibir_operacion(cliente_fd);
		log_info(logger, "Codigo OP: %d", cod_op);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
