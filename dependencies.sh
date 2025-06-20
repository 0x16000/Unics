#!/bin/bash

###########################################################
#                                                         #
#               UNICS DEPENDENCIES INSTALLER              #
#                                                         #
#                  Version 1.0 - 2025                     #
#                                                         #
###########################################################

# Colors for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Dependencies for each distro
declare -A ARCH_DEPS=(
    [nasm]="nasm"
    [xorriso]="xorriso"
    [qemu]="qemu-system-x86"
)

declare -A UBUNTU_DEPS=(
    [nasm]="nasm"
    [xorriso]="xorriso"
    [qemu]="qemu-system"
)

# Function to display error messages
error() {
    echo -e "${RED}[ERROR]${NC} $1" >&2
}

# Function to display success messages
success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

# Function to display info messages
info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

# Function to display warning messages
warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install dependencies for Arch
install_arch() {
    info "Updating pacman and installing dependencies for Arch..."
    sudo pacman -Syu --noconfirm || {
        error "Failed to update system packages"
        return 1
    }
    
    for pkg in "${ARCH_DEPS[@]}"; do
        if ! sudo pacman -S --noconfirm "$pkg"; then
            error "Failed to install $pkg"
            return 1
        fi
    done
}

# Function to install dependencies for Ubuntu
install_ubuntu() {
    info "Updating apt and installing dependencies for Ubuntu..."
    sudo apt update -y || {
        error "Failed to update package lists"
        return 1
    }
    
    for pkg in "${UBUNTU_DEPS[@]}"; do
        if ! sudo apt install -y "$pkg"; then
            error "Failed to install $pkg"
            return 1
        fi
    done
}

# Main installation function
install_dependencies() {
    local distro="$1"
    
    case "$distro" in
        arch|Arch|ARCH)
            install_arch || return 1
            ;;
        ubuntu|Ubuntu|UBUNTU)
            install_ubuntu || return 1
            ;;
        *)
            error "Unsupported distribution: $distro"
            return 1
            ;;
    esac
    
    return 0
}

# Verify sudo privileges
check_sudo() {
    if ! sudo -v; then
        error "You need sudo privileges to run this script"
        exit 1
    fi
}

# Main execution
main() {
    echo -e "\n${BLUE}UNICS Dependencies Installer${NC}\n"
    
    # Prompt for installation confirmation
    read -rp "Do you want to install the dependencies? (yes/no) " answer
    case "${answer,,}" in
        y|yes)
            ;;
        *)
            info "Installation cancelled by user."
            exit 0
            ;;
    esac
    
    # Detect distribution if not provided
    local detected_distro=""
    if command_exists pacman; then
        detected_distro="arch"
    elif command_exists apt; then
        detected_distro="ubuntu"
    fi
    
    # Prompt for distribution selection
    if [[ -n "$detected_distro" ]]; then
        read -rp "Detected ${detected_distro^}. Is this correct? (yes/no) " confirm
        if [[ "${confirm,,}" =~ ^(y|yes)$ ]]; then
            distro="$detected_distro"
        else
            distro=""
        fi
    fi
    
    if [[ -z "$distro" ]]; then
        read -rp "Which distro are you using? (arch/ubuntu) " distro
    fi
    
    # Check sudo before proceeding
    check_sudo
    
    # Install dependencies
    if install_dependencies "${distro,,}"; then
        success "All dependencies installed successfully!"
    else
        error "Failed to install all dependencies"
        exit 1
    fi
}

# Run main function
main