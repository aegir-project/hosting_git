Hosting site Git
================

This module extends drush/provision with 'git commands' in a site directory.

Supported commands are:

- git clone
- git pull (depends on provision_git)
- git checkout


Installation
============

1. Install as any other Drupal module into your hostmaster site.
2. The file hosting_git.drush.inc should be copied/linked to the drush installation.
An easy way is to create a symlink in ~/.drush/ to e.g. /var/aegir/clients/admin/hostmaster.example.com/modules/hosting_git/.
3. Make sure you have the drush module provision_git installed (drush dl provision_git)


WARNING: If you make code modifications on the server, don't forget to commit them. They will be lost otherwise.

Webhooks
=======

Provides a way for environments to stay up to date with the git repository.

Each environments can configure to Pull on Queue or Pull on URL Callback.

Pull on Queue will trigger Pull Code tasks on a regular basis using Hosting
Queues.  Pull on URL Callback provides a URL that you can add to your git host
to ping on receiving a commit.


GitHub Setup
------------

1. Visit your repos page: http://github.com/YOURNAME/YOURREPO
2. Click "Settings".
3. Click "Service Hooks".
4. Click "WebHook URLs"
5. Copy and paste your project's Git Pull Trigger URL into the URL field of the
   WebHook URLs page.
6. Click "Test Hook" to run a test, then check your DevShop project to ensure a
   Pull Code task was triggered.