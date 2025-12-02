// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <locale.h>
#include <getopt.h>

typedef enum {
  NONE = -1,
  ECONF,
  ERRNO,
  PAM
} command_t;

struct entry {
  const char *name;
  int code;
};

#include <errno.h>
#include "errno_data.h"

#include <libeconf.h>
#include "econf_data.h"

#include <security/_pam_types.h>
#include "pam_data.h"

#include "basics.h"

static void
print_usage(FILE *stream)
{
  fputs("Usage: error-codes <command> [options] | [--help] | [--version]\n", stream);
}

static void
print_help(void)
{
  fputs("error-codes - lookup error codes and their description.\n\n", stdout);
  print_usage(stdout);

  fputs("Commands: econf, errno, pam\n\n", stdout);

  fputs("Options for econf, errno and pam:\n", stdout);
  fputs("  <name-or-code>                    Print information about error name or code\n", stdout);
  fputs("  -l, --list                        List all error names, values and descriptions\n", stdout);
  fputs("  -s, --search <keyword...>         Search keywords in description\n", stdout);
  fputs("  -S, --search-locales <keyword...> Search keywords in all installed languages\n", stdout);
  fputs("\n", stdout);
  fputs("Generic options:\n", stdout);
  fputs("  -h, --help                        Give this help list\n", stdout);
  fputs("  -v, --version                     Print program version\n", stdout);
}

static void
print_error(void)
{
  fputs("Try `error-codes --help' for more information.\n", stderr);
}

static const char *
generic_strerror(command_t mode, int code)
{
  switch (mode)
    {
    case ECONF:
      return econf_errString((econf_err)code);
      break;
    case ERRNO:
      return strerror(code);
      break;
    case PAM:
      return pam_strerror(NULL, code);
      break;
    default:
      fprintf(stderr, "Unknown mode: %i\n", mode);
      print_error();
      exit(EINVAL);
    }
}

static void
print_entry(command_t mode, const struct entry *e)
{
  printf("%s - %i - %s\n", e->name, e->code,
	 generic_strerror(mode, e->code));
}

static const struct entry *
entry_from_name(const struct entry *list, const char *name)
{
  for (size_t i = 0; list[i].name != NULL; ++i)
    if (strcaseeq(list[i].name, name))
      return &list[i];

  return NULL;
}

static const struct entry *
entry_from_code(const struct entry *list, int code)
{
  for (size_t i = 0; list[i].name != NULL; ++i)
    if (list[i].code == code)
      return &list[i];

  return NULL;
}

static bool
matches(const char *desc, const char **words)
{
  for (size_t i = 0; words[i] != NULL; ++i)
    {
      if (strcasestr(desc, words[i]) == NULL)
	return false;
    }
  return true;
}

static void
search_words(command_t mode, const struct entry *list, const char **words)
{
  for (size_t i = 0; list[i].name != NULL; ++i)
    {
      if (matches(generic_strerror(mode, list[i].code), words))
	print_entry(mode, &list[i]);
    }
}

static int
search_words_locale(command_t mode, const struct entry *list, const char **words)
{
  FILE *fp;
  char locale[100]; /* static buffers are bad, but no locale should have 100 characters */
  int r;

  fp = popen("locale -a", "r");
  if (fp == NULL)
    {
      r = -errno;
      fprintf(stderr, "ERROR: 'locale -a' failed: %s\n", strerror(-r));
      return r;
    }

  while (fgets(locale, sizeof(locale), fp) != NULL)
    {
      size_t nl = strcspn(locale, "\n");

      if (nl)
	locale[nl] = '\0';

      for (size_t i = 0; list[i].name != NULL; ++i)
	{
	  const char *oldlocale = setlocale(LC_ALL, locale);
	  if (oldlocale == NULL)
	    {
	      printf("WARNING: locale '%s' does not work\n", locale);
	      continue;
	    }
	  if (matches(generic_strerror(mode, list[i].code), words))
	    print_entry(mode, &list[i]);
	  setlocale(LC_ALL, oldlocale);
	}
    }
  pclose(fp);

  return 0;
}

int
main(int argc, char **argv)
{
  const struct entry *list = NULL;
  int lflg = 0;
  int sflg = 0;
  int Sflg = 0;
  command_t mode = NONE;

  setlocale(LC_ALL, "");

  if (argc <= 1)
    {
      print_usage(stderr);
      exit(EINVAL);
    }

  if (streq(argv[1], "econf"))
    {
      mode = ECONF;
      list = econf_data;
    }
  else if (streq(argv[1], "errno"))
    {
      mode = ERRNO;
      list = errno_data;
    }
  else if (streq(argv[1], "pam"))
    {
      mode = PAM;
      list = pam_data;
    }
  else if (argv[1][0] != '-')
    {
      print_usage(stderr);
      exit(EINVAL);
    }

  if (mode != NONE)
    {
      --argc;
      ++argv;
    }

  while (1)
    {
      int c;
      int option_index = 0;
      static struct option long_options[] =
        {
	  {"list",           no_argument, NULL, 'l' },
	  {"search",         no_argument, NULL, 's' },
	  {"search-locales", no_argument, NULL, 'S' },
          {"help",           no_argument, NULL, 'h' },
          {"version",        no_argument, NULL, 'v' },
          {NULL,             0,           NULL, '\0'}
        };

      c = getopt_long (argc, argv, "lsShv",
                       long_options, &option_index);
      if (c == (-1))
        break;
      switch (c)
        {
	case 'l':
	  lflg = 1;
	  break;
	case 's':
	  sflg = 1;
	  break;
	case 'S':
	  Sflg = 1;
	  break;
        case 'h':
          print_help();
          return 0;
        case 'v':
          printf("error-codes (%s) %s\n", PACKAGE, VERSION);
          return 0;
        default:
	  fprintf(stderr, "Unknown option: '-%c'\n", c);
          print_error();
          return EINVAL;
        }
    }

  argc -= optind;
  argv += optind;

  if (lflg+sflg+Sflg > 1)
    {
      print_usage(stderr);
      return EINVAL;
    }

  if (lflg)
    {
      if (argc > 0)
	{
	  fprintf(stderr, "error-codes: too many arguments.\n");
	  print_error();
	  return EINVAL;
	}

      for (size_t i = 0; list[i].name != NULL; ++i)
	print_entry(mode, &list[i]);
    }
  else if (sflg || Sflg)
    {
      /* check that there is at least one word to search for */
      if (argc == 0)
	{
	  print_usage(stderr);
	  return EINVAL;
	}

      if (sflg)
	search_words(mode, list, (const char **)argv);
      else
	search_words_locale(mode, list, (const char **)argv);
    }
  else if (argc > 0)
    {
      for (int i = 0; i < argc; ++i)
	{
	  const char *arg = argv[i];
	  const struct entry *e = NULL;

	  if (isdigit(arg[0]))
	    e = entry_from_code(list, atoi(arg)); /* XXX strtol() */
	  else
	    e = entry_from_name(list, arg);

	  if (e == NULL)
	    printf("Not found: %s\n", arg);
	  else
	    print_entry(mode, e);
        }
    }
  else
    {
      print_usage(stderr);
      return EINVAL;
    }

  return 0;
}
