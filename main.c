#include "headers/exe_file.h"
#include "headers/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <spawn.h>

int main(int argc, char **args) {
  switch (argc) {
    case 1: {
      pandash();
      break;
    }
    case 2: {
      posix_spawn_file_actions_t action;
      posix_spawn_file_actions_init(&action);

      pid_t task_pid;
      char *command = args[1];
      char *command_args[] = {command, NULL};

      int spawn_error = posix_spawn(&task_pid, command, &action, NULL, command_args, NULL);
      if (spawn_error != 0) {
        fprintf(stderr, "Error executing command: %s\n", command);
        exit(EXIT_FAILURE);
      }

      waitpid(task_pid, NULL, 0);
      break;
    }
    default: {
      fprintf(stderr, "TOO MANY ARGS\n");
      exit(EXIT_FAILURE);
    }
  }
}
