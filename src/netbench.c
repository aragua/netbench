#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "server.h"
#include "client.h"


/* Flag set by ‘--verbose’. */
static int verbose_flag = 0;
static int server_mode = -1;
static char * sync_address = "192.168.0.1";
static char * mcast_address = "239.239.239.239";
static short port = 12345;


int check_args ( int argc, char ** argv )
{
    int c;
    while (1)
    {
        static struct option long_options[] =
        {
            /* These options set a flag. */
            {"verbose", no_argument,       &verbose_flag, 1},
            {"brief",   no_argument,       &verbose_flag, 0},
            {"server",   no_argument,       &server_mode, 1},
            {"client",   no_argument,       &server_mode, 0},
            /* These options don't set a flag.
                 We distinguish them by their indices. */
            {"sync",  required_argument, 0, 's'},
            {"multicast",  required_argument, 0, 'm'},
            {"port",  required_argument, 0, 'p'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "abc:d:f:",
                         long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
            printf ("option %s", long_options[option_index].name);
            if (optarg)
                printf (" with arg %s", optarg);
            printf ("\n");
            break;

        case 's':
            puts ("option -a\n");
            break;

        case 'm':
            puts ("option -b\n");
            break;

        case 'p':
            printf ("option -c with value `%s'\n", optarg);
            break;

        case '?':
            /* getopt_long already printed an error message. */
            break;

        default:
            abort ();
        }
    }

    /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
    if (verbose_flag)
        puts ("verbose flag is set");

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    return 0;
}

int main ( int argc, char ** argv )
{
    printf("Start of netbench project!!!\n");

    check_args( argc, argv );

    if ( server_mode < 0 )
    {
        fprintf( stderr, "Usage: netbench {--server/--client} ...\n");
        return EXIT_FAILURE;
    }
    else if ( server_mode == 0 )
        return start_server( mcast_address, port );
    else
        return start_client( sync_address, mcast_address, port );


    return EXIT_SUCCESS;
}
