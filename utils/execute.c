#include "../headers/execute.h"
#include "../headers/commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

void handle_built_in(Parsed *parsed_command) {
  if (strstr(parsed_command->command, "help")) {
    help();
    return;
  }
  if (strstr(parsed_command->command, "cd")) {
    if (!change_directory(parsed_command->flag_count == 0
                              ? NULL
                              : parsed_command->flags[1])) {
      fprintf(stderr, "COULD NOT CHANGE DIRECTORY\n");
    }
    return;
  }
  if (strstr(parsed_command->command, "exit")) {
    exit_pandash();
    return;
  }
  if (strstr(parsed_command->command, "pandash_talk")) {
    if (parsed_command->flag_count <= 2) {
      fprintf(stderr,
              "YOU NEED TO PROVIDE 2 VALUES, YOUR ID AND THE OTHER SHELL\n");
      return;
    }
    pid_t pid = fork();
    if (pid == -1) {
      return;
    } else if (pid == 0) {
      pandash_talk(parsed_command->flags[1], parsed_command->flags[2]);
      abort();
    } else {
      wait(NULL);
    }
    return;
  }
  if (strstr(parsed_command->command, "pandash_send")) {
    if (parsed_command->flag_count <= 2) {
      fprintf(stderr, "YOU NEED TO PROVIDE 2 VALUES, THE OTHER SHELL'S ID AND "
                      "YOUR QOUTED MESSAGE\n");
      return;
    }
    pid_t pid = fork();
    if (pid == -1) {
      return;
    } else if (pid == 0) {
      pandash_send(parsed_command->flags[1], parsed_command->flags[2]);
      abort();
    } else {
      wait(NULL);
    }
    return;
  }
}

int spawn_proc(Parsed *parsed_command) {
  pid_t pid = fork();
  if (pid == -1) {
    return -1;
  } else if (pid == 0) {
    execvp(parsed_command->flags[0], parsed_command->flags);
    perror("could not execute");
    abort();
  } else {
    wait(NULL);
  }
  return pid;
}

int pipes(Parsed *parsed_command) {
  pid_t pid;
  posix_spawn_file_actions_t action;
  posix_spawn_file_actions_init(&action);

  // Prepare file actions (if needed)
  // ...

  // Spawn the process
  if (posix_spawn(&pid, parsed_command->flags[0], &action, NULL, parsed_command->flags, NULL) != 0) {
    perror("could not execute");
    return -1;
  } else {
    waitpid(pid, NULL, 0);
  }

  // Clean up file actions (if needed)
  // ...

  return pid;
}

int spawn_proc_fork(int in, int out, Parsed *parsed_command) {
  int fd[2];
  posix_spawn_file_actions_t action;
  posix_spawn_file_actions_init(&action);

  // Prepare file actions (if needed)
  // ...

  // Spawn the process
  if (pipe(fd) != 0) {
    perror("could not create pipe");
    return -1;
  }
  if (posix_spawn_file_actions_adddup2(&action, fd[0], STDIN_FILENO) != 0 ||
      posix_spawn_file_actions_adddup2(&action, out, STDOUT_FILENO) != 0 ||
      posix_spawn_file_actions_addclose(&action, fd[1]) != 0) {
    perror("could not set file actions");
    return -1;
  }
  if (posix_spawn(NULL, parsed_command->flags[0], &action, NULL, parsed_command->flags, NULL) != 0) {
    perror("could not execute");
    return -1;
  }

  // Clean up file actions (if needed)
  // ...

  return 0;
}

int fork_pipes(Parsed *parsed_command) {
  int in = STDIN_FILENO;
  for (; parsed_command->command[0] != '\0'; parsed_command = parsed_command->next_command) {
    int fd[2];
    if (pipe(fd) != 0) {
      perror("could not create pipe");
      return -1;
    }
    if (spawn_proc_fork(in, fd[1], parsed_command) != 0) {
      perror("could not spawn process");
      return -1;
    }
    close(fd[1]);
    in = fd[0];
  }
  if (in != STDIN_FILENO)
    dup2(in, STDIN_FILENO);
  execvp(parsed_command->flags[0], parsed_command->flags);
  perror("could not execute");
  return -1;
}
