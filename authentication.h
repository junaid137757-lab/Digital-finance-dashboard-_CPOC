#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

/* User Management module: register, login,
   forgot/change password. Owns users.dat. */

void registerUser();
int loginUser();
void forgotPassword();
void changePassword();

#endif
