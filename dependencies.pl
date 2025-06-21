#!/usr/bin/perl

###########################################################
#                                                         #
#               UNICS DEPENDENCIES INSTALLER              #
#                                                         #
#                  Version 1.1 - 2025                     #
#                                                         #
###########################################################

use strict;
use warnings;
use Term::ANSIColor qw(:constants);
use POSIX qw(strftime);

# Colors for better output
my $RED = "\033[0;31m";
my $GREEN = "\033[0;32m";
my $YELLOW = "\033[1;33m";
my $BLUE = "\033[0;34m";
my $NC = "\033[0m"; # No Color

# Dependencies for each distro
my %ARCH_DEPS = (
    nasm => "nasm",
    xorriso => "xorriso",
    qemu => "qemu-system-x86",
    gcc_multilib => "gcc-multilib"
);

my %UBUNTU_DEPS = (
    nasm => "nasm",
    xorriso => "xorriso",
    qemu => "qemu-system",
    gcc_multilib => "gcc-multilib"
);

my %FEDORA_DEPS = (
    nasm => "nasm",
    xorriso => "xorriso",
    qemu => "qemu-system-x86",
    gcc_multilib => "glibc-devel.i686"  # Fedora equivalent for multilib dev
);

# Function to display error messages
sub error {
    print "${RED}[ERROR]${NC} $_[0]\n";
}

# Function to display success messages
sub success {
    print "${GREEN}[SUCCESS]${NC} $_[0]\n";
}

# Function to display info messages
sub info {
    print "${BLUE}[INFO]${NC} $_[0]\n";
}

# Function to display warning messages
sub warning {
    print "${YELLOW}[WARNING]${NC} $_[0]\n";
}

# Function to check if command exists
sub command_exists {
    my $cmd = shift;
    return system("command -v $cmd >/dev/null 2>&1") == 0;
}

# Function to install dependencies for Arch
sub install_arch {
    info("Updating pacman and installing dependencies for Arch...");
    if (system("sudo pacman -Syu --noconfirm") != 0) {
        error("Failed to update system packages");
        return 0;
    }

    foreach my $pkg (values %ARCH_DEPS) {
        if (system("sudo pacman -S --noconfirm $pkg") != 0) {
            error("Failed to install $pkg");
            return 0;
        }
    }
    return 1;
}

# Function to install dependencies for Ubuntu
sub install_ubuntu {
    info("Updating apt and installing dependencies for Ubuntu...");
    if (system("sudo apt update -y") != 0) {
        error("Failed to update package lists");
        return 0;
    }

    foreach my $pkg (values %UBUNTU_DEPS) {
        if (system("sudo apt install -y $pkg") != 0) {
            error("Failed to install $pkg");
            return 0;
        }
    }
    return 1;
}

# Function to install dependencies for Fedora
sub install_fedora {
    info("Updating dnf and installing dependencies for Fedora...");
    if (system("sudo dnf -y update") != 0) {
        error("Failed to update system packages");
        return 0;
    }

    foreach my $pkg (values %FEDORA_DEPS) {
        if (system("sudo dnf -y install $pkg") != 0) {
            error("Failed to install $pkg");
            return 0;
        }
    }
    return 1;
}

# Main installation function
sub install_dependencies {
    my $distro = lc(shift);

    if ($distro eq 'arch') {
        return install_arch();
    }
    elsif ($distro eq 'ubuntu') {
        return install_ubuntu();
    }
    elsif ($distro eq 'fedora') {
        return install_fedora();
    }
    else {
        error("Unsupported distribution: $distro");
        return 0;
    }
}

# Verify sudo privileges
sub check_sudo {
    if (system("sudo -v") != 0) {
        error("You need sudo privileges to run this script");
        exit(1);
    }
}

# Main execution
sub main {
    print "\n${BLUE}UNICS Dependencies Installer${NC}\n\n";

    # Prompt for installation confirmation
    print "Do you want to install the dependencies? (yes/no) ";
    my $answer = <STDIN>;
    chomp $answer;

    unless ($answer =~ /^y(es)?$/i) {
        info("Installation cancelled by user.");
        exit(0);
    }

    # Detect distribution if not provided
    my $detected_distro = "";
    if (command_exists("pacman")) {
        $detected_distro = "arch";
    }
    elsif (command_exists("apt")) {
        $detected_distro = "ubuntu";
    }
    elsif (command_exists("dnf")) {
        $detected_distro = "fedora";
    }

    my $distro;
    # Prompt for distribution selection
    if ($detected_distro) {
        print "Detected \u$detected_distro. Is this correct? (yes/no) ";
        my $confirm = <STDIN>;
        chomp $confirm;

        if ($confirm =~ /^y(es)?$/i) {
            $distro = $detected_distro;
        }
    }

    unless ($distro) {
        print "Which distro are you using? (arch/ubuntu/fedora) ";
        $distro = <STDIN>;
        chomp $distro;
    }

    # Check sudo before proceeding
    check_sudo();

    # Install dependencies
    if (install_dependencies(lc($distro))) {
        success("All dependencies installed successfully!");
    }
    else {
        error("Failed to install all dependencies");
        exit(1);
    }
}

# Run main function
main();
