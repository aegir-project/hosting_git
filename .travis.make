core = 7.x
api = 2

projects[drupal][type] = "core"

# Load 3.x branch of hostmaster.
projects[hostmaster][type] = "profile"
projects[hostmaster][version] = "3.x"

# Override source of hosting module download.
# .travis.yml puts hosting module at /source/hosting_git
projects[hosting_git][type] = "module"
projects[hosting_git][subdir] = "aegir"
projects[hosting_git][download][type] = "copy"
projects[hosting_git][download][url] = "/source/hosting_git"
