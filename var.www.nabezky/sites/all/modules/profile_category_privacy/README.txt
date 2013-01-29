$Id$ 

INSTALL
--------------------------------------------------------------------------------
Drupal 6 - extract into subdirectory of sites/all/modules

Enable the module.  You will also need to be using the 'Profiles' module.


Configuration
--------------------------------------------------------------------------------
User roles will, by default, be unable to see all profile information for other
users unless they have the 'administer users' permission set.  To grant permission
to view specific categories to your defined user roles, use admin/users/permissions
and check the appropriate options.

You need to create at least one category of information using the 'Profile' module 
for this module to have any effect or for the new permissions to appear in the 
administration section.