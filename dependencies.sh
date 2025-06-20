#!/bin/sh

###########################################################
#                                                         #
#                                                         #
#             UNICS DEPENDENCIES INSTALLER                #
#                                                         #
#                      20-6-2025                          #
#                                                         #
#                                                         #  
###########################################################

printf "Do you want to install the dependencies? (yes/no) "
read answer

case "$answer" in
  yes|YES|Yes)
    ;;
  *)
    printf "Installation cancelled.\n"
    exit 0
    ;;
esac

printf "Which distro are you using? (arch/ubuntu) "
read distro

case "$distro" in
  arch|Arch|ARCH)
    printf "Updating pacman and installing dependencies...\n"
    sudo pacman -Syu --noconfirm nasm xorriso qemu-system-x86
    ;;
  ubuntu|Ubuntu|UBUNTU)
    printf "Updating apt and installing dependencies...\n"
    sudo apt update
    sudo apt install -y nasm xorriso qemu-system
    ;;
  *)
    printf "Unsupported distro: %s\n" "$distro"
    exit 1
    ;;
esac

printf "Installation completed.\n"

