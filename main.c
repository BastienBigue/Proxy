#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define BACKLOG 100

//=========================================================================================================//

/*

The function select_port enables the user to choose the port on which he wants to be connected.
While he does not write a correct value of port, we ask him again.

*/

int* select_port ()
{
    int port = 0;
    int* pointeur_sur_port;
    pointeur_sur_port = &port;
    int ok = 0;
    int resultat;
    printf("Choose a port number between 1000 and 65535:\n");
    while (!ok)
    {
        resultat = scanf("%d%*[^\n]", &port);
        if (!resultat)
        {
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF); /* Reset of the buffer */
            printf("Wrong Input\n");
            printf("Choose a port number between 1000 and 65535:\n");
        }
        else
        {
            printf("Port : %d\n",port);
            if ( port < 1024 || port > 65535)
            {
                printf("Wrong Input\n");
                printf("Choose a port number between 1000 and 65535:\n");
            }
            else
            {
                getchar();
                printf("Input accepted\n");
                printf("The port number is : %d\n", port);
                ok = 1;
            }
        }
    }

    return pointeur_sur_port;
}
//=========================================================================================================//



//=========================================================================================================//
/*

Find_word enables us to detect if "word" is present in "message". It returns '1' if the word is present.

*/
int find_word(const char*word, char* message)
{
    int i;
    int counter = 0;
    int not_found = 1;
    int found=0;
    for (i=0; i<strlen(message); i++)
    {
        if ((message[i]==word[counter]) && (not_found))
        {
            if (message[i]==word[strlen(word)-1])
            {
                not_found = 0;
            }
            else
            {
                counter++;
            }
        }
        else if ((not_found) && (message[i]!=word[counter]))
        {
            counter=0;
        }
    }
    if (not_found)
    {
        found= 0;
    }
    else
    {
        found =1;
    }
    return found;
}
//=========================================================================================================//



//=========================================================================================================//

/*

word_filtering checks if several words are in the "content_to_filter". Returns '1' if the one of the word is present.

*/

int word_filtering(char* content_to_filter)
// --------FEATURE 3 & 4: Filtering of URL and content of the pages ------------//
{
    int bad_word_found = 0 ;

    if (find_word("SpongeBob",content_to_filter)==1)
    {
        bad_word_found = 1 ;
    }
    if (find_word("BritneySpears",content_to_filter)==1)                /* We look for all these words in the content_to_filter*/
    {
        /* We use the find_word function which returns 1 if the word is found */
        bad_word_found = 1 ;
    }
    if (find_word("ParisHilton",content_to_filter)==1)
    {
        bad_word_found = 1 ;
    }
    if (find_word("Norrkoping",content_to_filter)==1)
    {
        bad_word_found = 1 ;
    }

    return bad_word_found ;
}
//=========================================================================================================//



//=========================================================================================================//
/*

str_sub extract from "s" the char*  between the indices "start" and "end". It returns this char*

*/
char *str_sub (const char *s, unsigned int start, unsigned int end)
{
    char *new_s = NULL;

    if (s != NULL && start < end)
    {
        /* (1)*/
        new_s = malloc (sizeof (*new_s) * (end - start + 2));
        if (new_s != NULL)
        {
            int i;

            /* (2) */
            for (i = start; i <= end; i++)
            {
                /* (3) */
                new_s[i-start] = s[i];
            }
            new_s[i-start] = '\0';
        }
        else
        {
            fprintf (stderr, "Storage Insufficient\n");
            exit (EXIT_FAILURE);
        }
    }
    return new_s;
}
//=========================================================================================================//


//=========================================================================================================//

/*

Request_Manipulation enables us to modify the request in order to make it understandable for the server.
It's here we handle the HTTP 1.1 and HTTP 1.0 :
    -If the request is made with HTTP 1.0, we had nothing to do because there are not many headers with this mode.

    -If the request is made with HTTP 1.1, we have handled two browsers : Firefox and Chromium.
    These two browsers send to the proxy two requests which have different forms.
    We will manipulate these requests in order to send to the server a request under this form :
            GET HTTP/1.1
            Host:
            Connection: close
            User-Agent:
            Accept:
            Accept-Language:
            Accept-Encoding:
            Cookie:
            (and others...)

        -- For Firefox, we have reordered the headers. Moreover, we change the "Connection : keep-alive" into "Connection : close".
        -- For Chromium, we have removed the "Proxy-Connection" header. We also have changed the "Connection : keep-alive" into "Connection : close".

*/
void Request_Manipulation(char *message,char* reponse[25000], char *URL)
{

    char *coupe1;

    if (message[0]=='G')
    {
        if (strstr(message, "HTTP/1.1") != NULL)    /* MODIFICATION CONCERNING THE HTTP 1.1 METHOD */
        {
            // --------FEATURE 1 : HTTP 1.0 & HTTP 1.1------------//
            // --------FEATURE 2 : Handle Client-Server HTTP GET------------//
            strcat(reponse,"GET ");
            coupe1=strstr(message,"http://");
            coupe1=str_sub(coupe1,7,strlen(coupe1));                    /* Here we modify the beginning of the request in order to have : */
            coupe1=strstr(coupe1,"/");                                  /* GET PATH_TO_THE_REQUESTED_PAGE HTTP/1.1 */
            strcat(reponse,coupe1);                                     /* Host : */
            coupe1=strstr(reponse,"User");

            if (strstr(coupe1, "Firefox") != NULL)              /* MODIFICATION CONCERNING THE MOZILLA FIREFOX BROWSER */
            // --------FEATURE 6 : Compatible with several browsers ------------//
            {
                strcpy(coupe1,"\0");
                strcat(reponse, "Connection: close\r\n");
                coupe1=strstr(message,"User");
                strcat(reponse,coupe1);                                  /* Here we modify the "Connection" into Connection : close */
                coupe1=strstr(reponse,"Connection: keep-alive");
                strcpy(coupe1,"\0");
                coupe1 = strstr(message,"Connection: keep-alive");
                coupe1 = strstr(coupe1,"\r\n");                          /* Then, we add the others headers without the "Connection" one */
                coupe1=str_sub(coupe1,2,strlen(coupe1));
                strcat(reponse,coupe1);
            }
            else if (strstr(coupe1, "Ubuntu Chromium") != NULL)   /* MODIFICATION CONCERNING THE UBUNTU CHROMIUM BROWSER*/
            // --------FEATURE 6 : Compatible with several browsers ------------//
            {
                coupe1=strstr(reponse, "Proxy-Connection") ;
                strcpy(coupe1, "\0") ;                                   /* We removes the "Proxy-Connection" header and replace it by "Connection : close" */
                strcat(reponse, "Connection: close\r\n");
                coupe1=strstr(message,"Proxy-Connection");
                coupe1=strstr(coupe1, "\r\n") ;
                coupe1=str_sub(coupe1,2,strlen(coupe1));                /* Then, we add the others headers without the "Proxy-connection" one */
                strcat(reponse,coupe1);
            }
        }
        else if (strstr(message, "HTTP/1.0") != NULL)   /* MODIFICATION CONCERNING THE HTTP 1.0 METHOD */
        {
            // --------FEATURE 1 : HTTP 1.0 & HTTP 1.1------------//
            strcat(reponse,"GET ");
            coupe1=strstr(message,"http://");
            coupe1=str_sub(coupe1,7,strlen(coupe1));                    /* Here we modify the beginning of the request in order to have : */
            coupe1=strstr(coupe1,"/");                                  /* GET PATH_TO_THE_REQUESTED_PAGE HTTP/1.1 */
            strcat(reponse,coupe1);
            coupe1=strstr(reponse,"User");
            strcpy(coupe1,"\0");
            coupe1=strstr(message,"User");
            strcat(reponse,coupe1);                                     /* Then, we add the others HTTP 1.0 headers */
        }
        memset(coupe1,0, sizeof coupe1);
    }
}
//=========================================================================================================//



//=========================================================================================================//
/*

get_url_host find the URL of the page requested in "httpRequest". It is written in the variable "hostname".

*/

void get_url_host (char* httpRequest, char* hostname)

{
    int i;
    char* coupe1;
        strcpy(hostname,httpRequest);
        coupe1 = strstr(hostname,"Host");
        strcpy(hostname,coupe1);
        coupe1= strstr(hostname," ");
        strcpy(hostname,coupe1);
        coupe1 = strstr(hostname,"\r\n");
        strcpy(coupe1,"\0");
        coupe1 = str_sub(hostname,1,strlen(hostname));
        strcpy(hostname,coupe1);

}
//=========================================================================================================//



//=========================================================================================================//
/*

call_dns returns the IP of the server based on the URL address.

*/
char* call_dns(char* url_host)

{
    char* IP_host=NULL;
    struct hostent* h;
    h = gethostbyname(url_host);

    if ((h = gethostbyname(url_host)) == NULL) /* get_host_by_name calls the DNS Server */
    {
        printf("Error in call_dns\n");
    }
    else
    {
        IP_host = inet_ntoa(*((struct in_addr *)h->h_addr)); /* returns host_IP */
    }
    return IP_host;
}
//=========================================================================================================//


//=========================================================================================================//
/*

This is the client part of the proxy.
We receive the "httpRequest" and the socket which has permitted the establishment of the connection with the browser.
In the order, we realize :

1. Filtering of the URL.
    If the URL is OK, we do :
        2. Establishment of the connection.
        3. Manipulation of the request in order to make it understandable by the server.
        4. Sending of the request.
        5. Reception of the response packet by packet. Concatenation of the text.
        6. Filtering of the text of the whole response.
            If the content is OK we do :
                7. We close the current connection in order to open it again and to resend the same request.
                8. We receive the response and send it to the browser packet by packet.
            If the content is not OK :
                7.bis. We redirect the browser to the bad content page.
    If the URL is not OK, we do :
      2.bis. We redirect the browser to the bad URL page.

This programm is called in the server part, after we have established the connection with the browser.
There is one client part per acceptation of connection. This enables the proxy to handle several requests in the same time.

*/

void client_part (char* httpRequest, int csock)
{
    int sock;
    int taille;
    struct sockaddr_in sin;
    int sock_err = -1;
    int bad_word_detected  = 0;
    /*Création de la socket*/
    sock = socket(AF_INET, SOCK_STREAM, 0);
    char buffer1 [1000000];
    char reponse [1000000];
    char url[100000];
    const char redirection_url[10000] = "HTTP/1.1 302 Found\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error1.html\r\n\r\n";
    const char redirection_content[10000] = "HTTP/1.1 302 Found\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error2.html\r\n\r\n" ;
    char buffer2[100000];
    char to_be_filtered[1000000] ;

    get_url_host(httpRequest,url); /* The URL address of the website is stored is the variable url */

    if (word_filtering(httpRequest) == 0)
    // --------FEATURE 3: Filtering of the URL------------//
    {
        /* If the URL does not contains bad words, we can continue*/
        sin.sin_addr.s_addr = inet_addr(call_dns(url)); /* call_dns(url) returns the IP address of the server */
        sin.sin_family = AF_INET;
        sin.sin_port = htons(80);


        if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) != sock_err)
        {  /* Connection succeeded */
            printf("Connection to %s on port %d\n\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));

            Request_Manipulation(httpRequest,reponse,url);
            /* This function handles HTTP 1.0 and HTTP 1.1. It also handles the two browsers Mozilla Firefox and Ubuntu Chromium.
            This function modifies the request sent by the browser to the proxy to make it understanble by the server.*/

            if (send(sock, reponse, strlen(reponse), 0) != sock_err)
            {
                memset(url,0,sizeof url);
                taille = recv(sock, buffer2, 100000,0 );

                if (strstr(buffer2,"Content-Type: text")!=NULL && strstr(buffer2,"Content-Encoding:")==NULL)
                //--------FEATURE 8: Smart filtering : we only concatenate text and non-encoded content in order to filter it------------//
                {
                    strcat(to_be_filtered, buffer2) ;
                    memset(buffer2,0,sizeof buffer2);
                    while (taille !=0)          /* We concatenate the response in order to filter it later */
                    {
                        strcat(to_be_filtered, buffer2) ;
                        memset(buffer2,0,sizeof buffer2) ;
                        taille = recv(sock, buffer2, 100000,0 );
                    }
                }

                memset(buffer2,0,sizeof buffer2);
                if (word_filtering(to_be_filtered)==1)
                // --------FEATURE 4 & 8: Filtering of the text non-encoded content------------//
                {
                    /* If the content contains bad words, we redirect the browser to the bad content redirection page */
                    if (send(csock, redirection_content, strlen(redirection_content),0)!=-1)
                    {
                        printf("Bad content detected !!\n\nRedirection sent to the browser:\n\n%s\n\n",redirection_content);
                    }
                    else
                    {
                        printf("Error in the sending of the redirection of the bad content\n\n");
                        perror(send);
                    }
                    memset(reponse,0, sizeof reponse);
                }
                else                                       /* If the content does not contain bad words, we can continue*/
                {
                    close(sock);
                    sock = socket(AF_INET, SOCK_STREAM, 0);
                    if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) != sock_err)
                    {
                        if (send(sock, reponse, strlen(reponse), 0) != sock_err)
                        {
                        // --------FEATURE 5: No limit on the size of the data transmitted------------//
                            printf("Request sent to the server:\n\n%s\n\n", reponse);
                            taille = recv(sock, buffer1, 1000000, 0);
                            /* The buffer is much more large than the size of one packet */
                            while (taille !=0)
                            {
                                if (send(csock, buffer1, taille,0)!= -1)
                                /* When we get one packet, we send it immediately to the browser : we do not accumulate the packet, so there is no limit of size */
                                {
                                    printf("Packet sent by the socket %d:\n\n%s\n\n", csock ,buffer1);
                                    memset(buffer1,0, sizeof buffer1);
                                }
                                taille = recv(sock, buffer1, 1000000, 0);
                            }
                            memset(buffer1,0, sizeof buffer1);
                        }
                        else
                        {
                            printf("Error in send of good content\n\n");
                            perror(send);
                        }
                        memset(reponse,0, sizeof reponse);
                    }
                }
                memset(to_be_filtered, 0, sizeof to_be_filtered) ;
            }
            else
            {
                printf("Impossible to send the first request\n\n");
            }
        }
        else
        {
            printf("Impossible to connect to the server\n\n");
        }
    }
    else        /* If the URL contains bad words, we redirect the browser to the bad URL redirection page */
    {
        if (send(csock, redirection_url, strlen(redirection_url),0)!=-1)
        {
            printf("Bad URL detected !!\n\Redirection sent to the browser:\n%s\n",redirection_url);
        }
        else
        {
            printf("Error in the sending of the redirection of bad url\n\n");
            perror(send);
        }
    }
}
//=========================================================================================================//



//=========================================================================================================//

/* SERVER PART */
int server_part ()
{
    /* Initialization of the variables */
    struct sockaddr_in sin; /* Proxy Informations */
    char buffer[1000000];
    char requette_totale[1000000];
    struct sockaddr_in csin; /* Client Informations */
    int sock, csock;
    int sinsize = sizeof csin;
    int port = 0;
    pid_t pid ;
    int child_status ;
    int optval = 1 ;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    port = *select_port();
    //---------FEATURE 7: Allow the user to select the proxy port------------//

    if (sock == -1)
    {
        perror("socket()");
        exit(errno);
    }
    else
    {
        printf("The socket %d is now open with the TCP/IP protocol\n", sock);
        /* Server address configuration */
        sin.sin_addr.s_addr = INADDR_ANY; /* Automatic IP adress */
        sin.sin_family = AF_INET; /* IP Protocol */
        sin.sin_port = htons(port); /* Allocation of the port */
        if (bind (sock, (struct sockaddr *) &sin, sizeof sin) == -1)
        {
            perror("bind()");
            exit(errno);
        }
        else
        {
            if (listen(sock, BACKLOG) == -1) /*Listening */
            {
                perror("listen()");
                exit(errno);
            }
            else
            {
                printf("Listening...\n");
                while (1)
                {

                    csock = accept(sock, (struct sockaddr *)&csin, &sinsize); /* Exchange socket */
                    if (csock == -1)
                    {
                        perror("accept()");
                        exit(errno);
                    }
                    else
                    {
                    /* When a connection is accepted, we create a child process which will handle it.
                    This enables the proxy to get several connections and to handle them in the same time. */
                        pid = fork() ;
                        if (pid == 0)
                        {
                            close(sock) ;
                            printf("A client with the socket %d connect to the server from %s:%d\n\n", csock, inet_ntoa(csin.sin_addr),htons(csin.sin_port)); /*affichage des paramètres du client */
                            if (recv(csock, buffer, 1000000, 0) == -1)
                            {
                                perror("recv()");
                                exit(errno);
                            }
                            else
                            {
                                printf("Message received by the adresse %s:%d:\n\n%s\n\n",inet_ntoa(csin.sin_addr),htons(csin.sin_port),buffer);
                                if (buffer[0]=='G')
                                /* Once the proxy receive the request from the client, it checks if this is an HTTP (GET) or an HTTPs (CONNECT)
                                If it is a GET method, we can start to treat it in the client part.
                                If it is a CONNECT method, we do not have to handle it so we return an error (405, Method not allowed) message to the browser.*/
                                {
                                    client_part(buffer,csock);
                                }
                                else
                                {
                                    printf("Incorrect HTTP method\n\n");
                                    if (send(csock,"HTTP/1.1 405 Method Not Allowed\r\n\r\n", strlen("HTTP/1.1 405 Method Not Allowed\r\n\r\n"),0)!= -1)
                                    {
                                        printf("The wrong HTTP method redirection has been successful\n\n");
                                    }
                                    else
                                    {
                                        printf("Error in the sending of the wrong HTTP method redirection.\n\n");
                                        perror("send()");
                                    }
                                }
                            }
                            exit(2) ;
                        }
                        else if (pid < 0)
                        {
                            perror("fork()") ;
                            exit(errno) ;
                        }
                        else if (pid > 0)
                        {
                            close(csock) ;
                            wait (&child_status);
                            if (WIFEXITED (child_status))
                            {
                                printf ("The whole content has been successfully transmitted. the child process exited normally, with exit code %d\n\n", WEXITSTATUS (child_status));
                            }
                            else
                            {
                                printf ("An error occured in the child process. It exited abnormally\n\n");
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
//=========================================================================================================//



//=========================================================================================================//
int main()
{
    server_part();
    return 0;
}
//=========================================================================================================//
