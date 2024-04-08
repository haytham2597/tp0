#include"utils.h"

//#ifndef _ERRNO_H
#include "errno.h"
//#endif

t_log* logger;

int print_error(char* from){
	printf("Error [%s]: %i\n", from, errno);
	return -1;
}

int iniciar_servidor(void)
{
	int sock_serv;
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;
	hints.ai_flags = 1;

	getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	//https://pubs.opengroup.org/onlinepubs/9699919799/functions/socket.html diff 0 return errno 
	sock_serv = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	
	if(sock_serv < 0)
		return print_error("socket");
	
	//servinfo->ai_addr is a struct of sockaddr...
	if(bind(sock_serv, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
		return print_error("bind");

	log_info(logger, "Socket asociado correctamente");
	
	if(listen(sock_serv, SOMAXCONN) < 0)
		return print_error("listen");


	/*if(accept(sock_serv, (sockaddr*)servinfo, sizeof(servinfo)) != 0)
		return print_error();*/

	// Creamos el socket de escucha del servidor
	// Asociamos el socket a un puerto
	// Escuchamos las conexiones entrantes

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");
	
	return sock_serv;
}

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente;
	struct addrinfo hints, *clientaddr;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 1;
	//hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PUERTO, &hints, &clientaddr);

	//sock_serv = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	//socket_cliente = accept(socket_servidor, clientaddr->ai_addr,clientaddr->ai_addrlen);
	socket_cliente = accept(socket_servidor,NULL,NULL);
	if(socket_cliente == -1){
		return print_error("wait client");
	}
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

/*int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) >= 0){
		printf("Recibi operacion %d\n", &cod_op);
		return cod_op;
	}
	else
	{
		print_error("recv_op");
		close(socket_cliente);
		return -1;
	}
}*/

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}


//El buffer recibe 2 entradas; un entero que indica tamaño del buffer y el buffer
void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
