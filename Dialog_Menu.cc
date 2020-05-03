/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/29/2020 at 01:20 AM
 * 
 * Dialog_Menu.cc
 * 
 * This is a Dialog Based UI for the Rain Alert Project
 * (CSC 4110 Spring Semester 2020)
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"
#include <dialog.h>
// Project_File for everything:

int main(void){
    int status;
    init_dialog(stdin, stdout);
    status = dialog_yesno(
	"Hello, in dialog-format",
	"Hello World!",
	0, 0);
    end_dialog();
    return status;
}
