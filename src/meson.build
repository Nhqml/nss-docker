sources = [
  'docker_nss.c',
  'docker_api.c',
  'utils.c',
]

nss_libvirt_lib = shared_library('nss_docker', sources,
  soversion: '2',
  install: true,
  gnu_symbol_visibility: 'hidden',
  dependencies: [
      dependency('libcurl', version: '>=7.80'),
      dependency('libcjson', version: '>=1.7.15'),
  ]
)
