project(
  'error-codes',
  'c',
  meson_version : '>= 0.61.0',
  default_options : [
                  'prefix=/usr',
                  'sysconfdir=/etc',
                  'localstatedir=/var',
                  'buildtype=debugoptimized',
  		  'default_library=shared',
		  'b_pie=true',
                  'b_lto=true',
		  'warning_level=2'],
  license : ['GPL-2.0-or-later'],
  version : '0.1.0',
)

conf = configuration_data()
conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('PACKAGE', meson.project_name())

cc = meson.get_compiler('c')
pkg = import('pkgconfig')
inc = include_directories(['include'])

add_project_arguments(['-D_GNU_SOURCE=1',
		       '-DXTSTRINGDEFINES',
		       '-D_FORTIFY_SOURCE=2',
                       '-D_FILE_OFFSET_BITS=64',
                       '-D_TIME_BITS=64'], language : 'c')

possible_cc_flags = [
		  '-fstack-protector-strong',
		  '-funwind-tables',
		  '-fasynchronous-unwind-tables',
		  '-fstack-clash-protection',
		  '-Werror=return-type',
		  '-Wbad-function-cast',
		  '-Wcast-align',
                  '-Wformat-security',
		  '-Winline',
		  '-Wmissing-declarations',
		  '-Wmissing-prototypes',
		  '-Wnested-externs',
		  '-Wshadow',
		  '-Wstrict-prototypes',
		  '-Wundef',
		  ]
add_project_arguments(cc.get_supported_arguments(possible_cc_flags), language : 'c')

bindir = get_option('bindir')

configure_file(output: 'config.h', configuration: conf)

libeconf = dependency('libeconf', required: true)
libpam = dependency('pam', required: true)

env = environment()
env.set('CC', ' '.join(cc.cmd_array()))

errno_data_h = custom_target(
  'generate_errno_data_h',
  output : 'errno_data.h',
  input : 'scripts/gen_errno_data_h.sh',
  command : [find_program('sh'), '@INPUT@', '@OUTPUT@'],
  env : env
)

econf_data_h = custom_target(
  'generate_econf_data_h',
  output : 'econf_data.h',
  input : 'scripts/gen_econf_data_h.sh',
  command : [find_program('sh'), '@INPUT@', '@OUTPUT@'],
  env : env
)

pam_data_h = custom_target(
  'generate_pam_data_h',
  output : 'pam_data.h',
  input : 'scripts/gen_pam_data_h.sh',
  command : [find_program('sh'), '@INPUT@', '@OUTPUT@'],
  env : env
)

executable('error-codes',
           sources : ['src/error-codes.c', errno_data_h, econf_data_h, pam_data_h],
           include_directories : inc,
           dependencies : [libpam, libeconf],
           install : true,
	   install_dir : bindir)

# Unit tests
#subdir('tests')

# Manual pages
subdir('man')
