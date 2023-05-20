#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdbool.h>
#include <signal.h>
int count_users = 0;
const char *program_name; 
double cpu_time_used = 0; 
int errorRequests = 0; 
#define BUF_SIZE        1024
#define SIZE 100
char eror_buff[BUF_SIZE] = {0};
unsigned int global_len;
#define CHECK_RESULT(res, msg)          \
do {                                    \
                     \
    if (res < 0) {     \
        errorRequests++;                \
        perror(msg);                    \
        exit(EXIT_FAILURE);             \
    }                                   \
} while (0)


//volatile sig_atomic_t exit_flag = 0;
void signal_handler(int __attribute__((unused))(signum)) {
    printf ("\nThank you for using my server \t");
    printf ("Exiting ...\n");
    exit(EXIT_SUCCESS);
}
void sigusr1_handler(int __attribute__((unused))(signum)) {
    fprintf(stderr, "The server has worked %lf of the time. Processed %d requests without errors and %d requests with errors\n",
            cpu_time_used, count_users, errorRequests);
    exit(EXIT_SUCCESS);
}
void daemonize() {
    // Установка обработчиков сигналов с помощью sigaction
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction(SIGCHLD) failed");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("sigaction(SIGTSTP) failed");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGTTOU, &sa, NULL) == -1) {
        perror("sigaction(SIGTTOU) failed");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGTTIN, &sa, NULL) == -1) {
        perror("sigaction(SIGTTIN) failed");
        exit(EXIT_FAILURE);
    }
    // Создание нового процесса
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Родительский процесс завершается
        exit(EXIT_SUCCESS);
    }

    // Создание новой сессии
    if (setsid() < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    // Игнорирование сигналов SIGHUP, SIGINT и SIGTERM
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        perror("sigaction(SIGHUP) failed");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT) failed");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction(SIGTERM) failed");
        exit(EXIT_FAILURE);
    }

    // Изменение текущей директории в корневую
    if (chdir("/") == -1) {
        perror("chdir failed");
        exit(EXIT_FAILURE);
    }

    // Закрытие стандартных файловых дескрипторов
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}


int main(int argc, char *argv[])
{ 
    time_t current_time;
    struct tm* time_info;
    char time_str[20];
    time(&current_time);
    program_name = argv[0];
    char* debug = getenv("LAB2DEBUG");
    char* ip_adrr = getenv("LAB2ADDR");
    char* log_path = getenv("LAB2LOGFILE");
    char* env_port = getenv("LAB2PORT");
    char* env_wait = getenv("LAB2WAIT");
    int option; 
    int c_sleep = 0;
    char *path_to_log = "/tmp/lab2.log";
    char *ip_val = "127.0.0.1";
    int port = 7777;
    bool help = false, version = false;
    // Натсройка структуры для 3 сигналов 
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    clock_t end;
    clock_t start;
    bool flag_ip = false;
    bool flag_port = false;
    bool flag_wait = false;
    bool flag_path_to_log = false;

    sa.sa_flags = 0;
    //
    // Насройка структуры для 1 пользовательского сигнала
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    // 
     while ((option = getopt(argc,argv, "w:hvl:da:p:")) != -1) {
        switch (option) {
            case 'w':
                if (optarg != NULL){
                    c_sleep = atoi(optarg);
                    flag_wait = true;
                    if (debug){
                    printf("Sleep time : %d\n", c_sleep);
                    }
                }  
                break;
            case 'l':
                if (optarg != NULL){
                    path_to_log = optarg;
                    flag_path_to_log = true;
                    if (debug){
                        printf("LOG : %s\n", path_to_log);
                    }
                }  
                break;
            case 'a':
                if (optarg != NULL){
                    ip_val = optarg;
                    flag_ip = true; 
                    if (debug){
                        printf("IP : %s\n", ip_val);
                    }
                }
                break;
            case 'p':
                if (optarg != NULL) {
                    port = atoi(optarg);
                    flag_port = true;
                    if (debug){
                        printf("PORT: %d\n", port);
                    }
            }
                break;
            case 'h':
                help = true;
                break;
            case 'v':
                version = true;
                break;
            case 'd':
                daemonize();
                break;
            case '?':
                printf("Oops, your guess is as good as mine, what happend ^_^ 0_o ");
                break;
        }
    }
    if (help){
        printf("Usage: %s [OPTION]...\n", program_name);
        printf("Options:\n");
        printf(" -h, display this help and exit\n");
        printf(" -v, output version information and exit\n");
        printf(" -a, set custom IP address\n");
        printf(" -w, set sleep time \n");
        printf(" -d, run the program in daemon mode \n");
        printf(" -l, path to log file \n");
        printf(" -p , set custom port address \n");
        exit(EXIT_SUCCESS);
    }
    if (version){
        printf("%s version 1.0\n", program_name);
        printf("info: The server that accepts the domain and returns the available ip addresses on it\n");
        printf("Author: Kopylov Nikita\n");
        printf("Group: N32511\n");
        printf("Lab: 2 variant 19\n");
        exit(EXIT_SUCCESS);
    }
    if (ip_adrr && !flag_ip){
        ip_val = ip_adrr;
        if (debug){
            printf("IP : %s\n", ip_val);
        }
    }   
    if (env_port && !flag_port){
        port = atoi(env_port);
        if (debug){
            printf("PORT: %d\n", port);
        }
    }
    if (log_path && !flag_path_to_log){
        path_to_log = log_path;
        if (debug){
            printf("LOG : %s\n", path_to_log);
        }
    } 
     if (env_wait && !flag_wait){
         c_sleep = atoi(env_wait);
        if (debug){
            printf("Wait time : %d\n", c_sleep);
        }
    } 
    
    int serverSocket;
    char buffer[BUF_SIZE] = {0};
    struct sockaddr_in serverAddr = {0};
    struct sockaddr_in clientAddr = {0};
    socklen_t __attribute__((unused))addr_size;
    
    int res;    
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    CHECK_RESULT(serverSocket, "socket");

    res = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    CHECK_RESULT(res, "setsockopt");
    //in_addr_t ip_addr = inet_addr(ip_val);
    if (inet_pton(AF_INET, ip_val, &(serverAddr.sin_addr)) <= 0) {
        printf("Error: Invalid IP address.\n");
        return 1;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    //serverAddr.sin_addr.s_addr = htonl(ip_addr);

    res = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    CHECK_RESULT(res, "bind");
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction(SIGUSR1) failed");
        return 1;
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT) failed");
        return 1;
    }

    // Обработка SIGTERM
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction(SIGTERM) failed");
        return 1;
    }

    // Обработка SIGQUIT
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("sigaction(SIGQUIT) failed");
        return 1;
    }
     if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("sigaction(SIGQUIT) failed");
        return 1;
    }
    while (1) {
        start = clock(); 

        if (c_sleep > 0){
            sleep(c_sleep);
        }
        unsigned int len = sizeof(clientAddr);
        global_len = len;
        bzero(buffer, sizeof(buffer));
        res = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &len);
        CHECK_RESULT(res, "recvfrom");
        count_users++;
        pid_t pid = fork();
        if (pid == 0){
            FILE* file = fopen(path_to_log, "w");
            fprintf(file,"A new client has connected\n");
            char* hostname = buffer;
            struct hostent* host = gethostbyname(hostname);
            if (host == NULL) {
                int res_fail = -1;
                fprintf(stderr, "Could not resolve hostname: %s\n", hostname);
                CHECK_RESULT(res_fail, "Could not resolve hostname");
                exit(EXIT_FAILURE);
            }
            struct in_addr **addr_list = (struct in_addr **)host->h_addr_list;
            int i = 0;
            while (addr_list[i] != NULL) {
                i++;
            }

            int numIPAddresses = i;
            if (debug){
                printf("Num: %d\n",numIPAddresses);
            }
            i = 0;
            res = sendto(serverSocket, &numIPAddresses, sizeof(numIPAddresses), 0, (const struct sockaddr*)&clientAddr, len);
            while (addr_list[i] != NULL) {
            char *ip = inet_ntoa(*addr_list[i]);
            if (debug){
                printf("Sending IP address: %s\n", ip);
            }
            res = sendto(serverSocket, ip, BUF_SIZE, 0, 
                (const struct sockaddr*)&clientAddr, len);
            CHECK_RESULT(res, "sendto");
            i++;
            }
            end = clock();
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            if (file == NULL) {
                printf("The file could not be opened.\n");
                exit(EXIT_FAILURE);
            }
            time_info = localtime(&current_time);  
            strftime(time_str, sizeof(time_str), "%d.%m.%y %H:%M:%S", time_info);
            if (debug){
                fprintf(stderr, "The server has worked %lf of the time. Processed %d requests without errors and %d requests with errors\n",
                    cpu_time_used, count_users, errorRequests);
            }
            else {
                fprintf(file, "%s :The server has worked %lf of the time. Processed %d requests without errors and %d requests with errors\n",
                    time_str,cpu_time_used, count_users, errorRequests);
                }
            fclose(file);
            exit(EXIT_SUCCESS);
        }
        else if (pid < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    }
    close(serverSocket);
   
    
    return 0;
}
