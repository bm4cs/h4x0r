int main(int argc, char *argv[]);
void *handler(void *p_sockfd);
int socket_printf(int c_sockfd, char *format, ...);
void fprintfsock(int c_sockfd, const char* format, ... );
bool test(void*);
