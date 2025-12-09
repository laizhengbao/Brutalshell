
#include "wrapper.h"

#include "yaml.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#ifndef BUFLEN
#define BUFLEN 256
#endif

#ifndef BSH_CONFIGPATH
#define BSH_CONFIGPATH "/etc/brutalshell/config.conf"
#endif

#ifndef BSH_DEFAULT_DAEMON_PATH
#define BSH_DEFAULT_DAEMON_PATH "/var/run/bsh/bsh.socket"
#endif

struct config _get_yaml( void *, struct config );

struct config get_configure( int argc, char **restrict argv ){

	register struct config cfg = {
		.logfd = 0,
		.argv = NULL,
		.daemon_method = 0,
		.desc = NULL,
		.len = 0,
	};

	register int fd;
	register void *file;

	register char *restrict cfg_path = BSH_CONFIGPATH;

	register char *restrict cfg_home;

	register ssize_t home_len;

	cfg.logfd = logfd = STDERR_FILENO;

	cfg.argv = NULL;

	if ( argc > 2 && !strcmp( "c", *( argv + 1 ) ) ){
		cfg_path = *( argv + 2 );
		cfg.argv = argv + 3;
	} else if ( argc > 1 && !strcmp( "?", *( argv + 1 ) ) ){
		usage( *argv );
		return cfg;
	}else{
		if ( argc > 1 ){
			cfg.argv = argv + 1;
		}
	}

#	ifdef DBG
	fprintf( stderr, "cfg: %s\n", cfg_path );
#	endif

	fd = open( cfg_path, O_RDONLY );
	if ( fd < 0 ){

		if ( ( fd = open( getenv( "BSH_CFG" ), O_RDONLY ) ) < 0 ){

			if ( ( fd = open( "config.conf", O_RDONLY ) ) >= 0 ){
				goto SUCCESS;
			}

			cfg.desc = getenv( "HOME" );
			if ( !cfg.desc ){
				goto RET;
			}

			home_len = strlen( cfg.desc );
			if ( home_len > PATH_MAX ){
				home_len = PATH_MAX;
			}

			cfg_home = malloc( PATH_MAX + 1 );
			memset( cfg_home, 0, PATH_MAX + 1 );
			cfg.desc = stpncpy( cfg_home, cfg.desc, home_len );
			strncpy( cfg.desc, "/.config/bsh/config.conf", PATH_MAX - home_len );

			cfg.desc = NULL;

			fd = open( cfg_home, O_RDONLY );
			free( cfg_home );
			if ( fd < 0 ){
				goto RET;
			}

		}
	}

SUCCESS:

	file = fdopen( fd, "r" );

	cfg = _get_yaml( file, cfg );

	fclose( file );

RET:

	if ( !cfg.desc ){
		cfg.daemon_method = 0;
		cfg.desc = strdup( BSH_DEFAULT_DAEMON_PATH );
	}

	if ( !cfg.argv || !*cfg.argv ){
		cfg.argv = malloc( sizeof( *cfg.argv ) << 1 );
		*( cfg.argv + 0 ) = strdup( "/bin/sh" );
		*( cfg.argv + 1 ) = NULL;
	}

#	ifdef DBG
	fprintf( stderr, "%s\n", *cfg.argv );
#	endif

	return cfg;

}

struct config _get_yaml( void *f, struct config cfg ){

	register int is_val;
	register int is_seq;
	register int is_arg;
	register void *k, *v;

	register size_t seq_len;

	register int done;

	typedef struct _n {
		struct _n *next;
		void *val;
	} node;

	register node *arg_root;
	register node *tmp;

	yaml_parser_t par;
	yaml_event_t eve;

	k = NULL;

	if ( !yaml_parser_initialize( &par ) ){
		goto RET;
	}

	yaml_parser_set_input_file( &par, f );

	done = 0;
	k = NULL;
	v = NULL;

	is_val = 0;
	is_seq = 0;
	is_arg = 0;
	seq_len = 0;
	arg_root = NULL;

	while ( !done ){
		if ( !yaml_parser_parse( &par, &eve ) ){
			break;
		}

		switch ( eve.type ){
			case YAML_SCALAR_EVENT:
				v = eve.data.scalar.value;


				if ( !is_val ){

					if ( k ){
						free( k );
					}

					k = strdup( v );
					is_val = 1;

					is_arg = k && !strcmp( k, "shell" );

#					ifdef DBG
					fprintf( stderr, "KEY: [%s]\n", v );
#					endif

				} else {
#					ifdef DBG
					fprintf( stderr, "VAL: %s\n", v );
#					endif

					if ( is_seq ){
						if ( is_arg ){

							/* PUSH */
							tmp = malloc( sizeof( *tmp ) );
							tmp -> next = arg_root;
							tmp -> val = strdup( v );
							arg_root = tmp;

							seq_len++;
						}
					} else if ( k && !strcmp( k, "method" ) ){
						cfg.daemon_method = atoi( v );
						is_val = 0;
					} else if ( k && !strcmp( k, "path" ) ){
						cfg.desc = strdup( v );
						cfg.len = strlen( v );
						is_val = 0;
					} else {
						is_val = 0;
					}
				}

				break;
			case YAML_SEQUENCE_START_EVENT:
				is_seq = 1;
				is_val = 1;
#				ifdef DBG
				fprintf( stderr, "SEQ START\n" );
#				endif
				break;
			case YAML_SEQUENCE_END_EVENT:
				is_val = 0;
				is_seq = 0;
#				ifdef DBG
				fprintf( stderr, "SEQ END\n" );
#				endif
				if ( seq_len && arg_root ){
					if ( is_arg ){
						cfg.argv = malloc( sizeof( *cfg.argv ) * ( seq_len + 1 ) );
						memset( cfg.argv, 0, sizeof( *cfg.argv ) * ( seq_len + 1 ) );
						is_arg = 0;
						while ( arg_root ){
							--seq_len;
							*( cfg.argv + seq_len ) = arg_root -> val;
							/* POP */
							tmp = arg_root;
							arg_root = arg_root -> next;
							free( tmp );
						}
					}
					seq_len = 0;
				}
				break;
			case YAML_MAPPING_START_EVENT:
				is_val = 0;
#				ifdef DBG
				fprintf( stderr, "MAP START\n" );
#				endif
				break;
			case YAML_MAPPING_END_EVENT:
#				ifdef DBG
				fprintf( stderr, "MAP END\n" );
#				endif
				break;

			case YAML_STREAM_END_EVENT:
				done = 1;
				break;

			default:
				break;
		}

		yaml_event_delete( &eve );
	}

	yaml_parser_delete( &par );

RET:

	if ( k ){
		free( k );
	}

	return cfg;
}
