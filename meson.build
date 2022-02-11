project(
  'nss-docker', 'c',
  version: '0.1',
  license: 'GPL-3.0-or-later',
  meson_version: '>=0.61.1',
  default_options: [
    'prefix=/usr',
    'buildtype=release',
    'warning_level=3',
    'werror=true',
    'b_lto=true',
    'b_lto_mode=thin',
    'b_ndebug=if-release',
    'strip=true' # only strip on install
  ],
)

subdir('src')

install_man('nss-docker.8')
