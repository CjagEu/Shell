/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell          
	(then type ^D to exit program)

**/

#include "job_control.h"   // remember to compile with module job_control.c 
#include "string.h"

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------

int main(void) {
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background;             /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/2];     /* command line (of 256) has max of 128 arguments */
	// probably useful variables:
	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;             /* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;				/* info processed by analyze_status() */

	ignore_terminal_signals();		//Voy a ignorar señales para que los hijos puedan establecer el control del terminal
	
	while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{   		
		setvbuf(stdout, NULL, _IONBF, 0);
		setvbuf(stderr, NULL, _IONBF, 0);
		printf("COMMAND->");
		fflush(stdout);

		
		get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */

		if(args[0]==NULL) continue;   // if empty command

		//Comprobar si es un comando interno 
		if(strcmp(args[0], "cd") == 0){
			printf("Ejecutando comando interno\n");
			chdir(args[1]);
			continue;
		}
		// the steps are:
		//	 (1) fork a child process using fork()
			 pid_fork = fork();
		//	 (2) the child process will invoke execvp()
			 if(pid_fork == 0){
				//Lo meto en un grupo de procesos, y entra automaticamente a segundo plano por estar en el grupo de procesos.
				new_process_group(getpid());		
				if(background == 0){
					set_terminal(getpid());		//Si no es background, le doy el poder.
					restore_terminal_signals();	//Antes de ejecutar el hijo, hago que escuche las señales.
					execvp(args[0], args);
			 		printf("ERROR, comando no puede ser ejecutado\n");
					exit(-1);
				}else{
					restore_terminal_signals();	//Antes de ejecutar el hijo, hago que escuche las señales.
			 		execvp(args[0], args);
			 		printf("ERROR, comando no puede ser ejecutado\n");
					exit(-1);
				}
			 }else{
		//	 (3) if background == 0, the parent will wait, otherwise continue 
		//	 (4) Shell shows a status message for processed command 
			 	if(background == 0){
					 waitpid(pid_fork, &status, WUNTRACED); //WUNTRACED (antes 0), es para trackear si ha finalizado bien, o se ha suspendido etc
					 //Tras esperar al hijo, le devuelvo el control a la shell
					 set_terminal(getpid());
				 	status_res = analyze_status(status, &info);
					 printf("Foreground pid: %d, command: %s, %s, info: %d \n", pid_fork,args[0], status_strings[status_res], info);
			 	}else{
					 printf("Background job running... pid: %d, command: %s \n", pid_fork, args[0]);
				 }
			 }
		//	 (5) loop returns to get_commnad() function


	} // end while
}

/*
	Tarea 2 Terminada
*/