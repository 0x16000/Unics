#!/usr/bin/perl

###########################################################
#                                                         #
#               UNICS DEPENDENCIES INSTALLER              #
#                                                         #
#                  Version 1.2 - 2025                     #
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

# Complete dependency lists for each distro
my %DEPS = (
    arch => {
        base => "base-devel",
        tools => "nasm xorriso grub",
        qemu => "qemu-system-x86",
        multilib => "lib32-glibc lib32-gcc-libs",
        extras => "mtools"
    },
    ubuntu => {
        base => "build-essential",
        tools => "nasm xorriso grub-pc-bin",
        qemu => "qemu-system-x86",
        multilib => "gcc-multilib libc6-dev-i386",
        extras => "mtools"
    },
    debian => {
        base => "build-essential",
        tools => "nasm xorriso grub-pc-bin",
        qemu => "qemu-system-x86",
        multilib => "gcc-multilib libc6-dev-i386",
        extras => "mtools"
    },
    fedora => {
        base => "gcc make automake",
        tools => "nasm xorriso grub2-tools",
        qemu => "qemu-system-x86",
        multilib => "glibc-devel.i686 libstdc++-devel.i686",
        extras => "mtools"
    },
    centos => {
        base => "gcc make automake",
        tools => "nasm xorriso grub2-tools",
        qemu => "qemu-system-x86",
        multilib => "glibc-devel.i686 libstdc++-devel.i686",
        extras => "mtools"
    },
    linuxmint => {
        base => "build-essential",
        tools => "nasm xorriso grub-pc-bin",
        qemu => "qemu-system-x86",
        multilib => "gcc-multilib libc6-dev-i386",
        extras => "mtools"
    }
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

# Function to detect Linux distribution
# Function to detect Linux distribution
sub detect_distro {
    if (-e "/etc/os-release") {
        open my $fh, '<', '/etc/os-release' or return undef;
        while (my $line = <$fh>) {
            if ($line =~ /^ID=(.+)/) {
                my $id = $1;
                $id =~ s/"//g;
                $id = lc($id);
                # Normalize mint/linuxmint
                $id = "linuxmint" if $id eq "mint";
                return $id;
            }
        }
        close $fh;
    }
    
    # Fallback detection methods
    if (command_exists("pacman")) { return "arch"; }
    if (command_exists("apt"))    { return "ubuntu"; }
    if (command_exists("dnf"))    { return "fedora"; }
    if (command_exists("yum"))    { return "centos"; }
    
    return undef;
}

# Function to install dependencies for a distro
sub install_deps {
    my $distro = shift;
    
    unless (exists $DEPS{$distro}) {
        error("Unsupported distribution: $distro");
        return 0;
    }
    
    my $pkg_cmd = $distro eq 'arch' ? 'pacman -S --noconfirm' :
                  $distro =~ /(ubuntu|debian|linuxmint)/ ? 'apt install -y' :
                  $distro =~ /(fedora|centos)/ ? 'dnf install -y' :
                  undef;
    
    unless ($pkg_cmd) {
        error("No package manager command for $distro");
        return 0;
    }
    
    # Update package lists first
    info("Updating package databases...");
    my $update_cmd = $distro eq 'arch' ? 'pacman -Syu --noconfirm' :
                     $distro =~ /(ubuntu|debian|linuxmint)/ ? 'apt update -y' :
                     $distro =~ /(fedora|centos)/ ? 'dnf update -y' :
                     undef;
    
    system("sudo $update_cmd") == 0 or do {
        error("Failed to update packages");
        return 0;
    };
    
    # Install packages in groups
    my @groups = qw(base tools qemu multilib extras);
    foreach my $group (@groups) {
        next unless $DEPS{$distro}{$group};
        
        info("Installing $group packages...");
        if (system("sudo $pkg_cmd $DEPS{$distro}{$group}") != 0) {
            error("Failed to install $group packages");
            return 0;
        }
    }
    
    return 1;
}

# Verify all required tools are installed
sub verify_installation {
    my @required_tools = qw(gcc nasm ld objcopy grub-mkrescue qemu-system-x86_64);
    my $all_ok = 1;
    
    info("Verifying installation...");
    
    foreach my $tool (@required_tools) {
        if (command_exists($tool)) {
            success("$tool found");
        } else {
            error("$tool not found in PATH");
            $all_ok = 0;
        }
    }
    
    # Check 32-bit support
    if (system("gcc -m32 -print-libgcc-file-name >/dev/null 2>&1") == 0) {
        success("32-bit compilation support available");
    } else {
        error("32-bit compilation not supported");
        $all_ok = 0;
    }
    
    return $all_ok;
}

# Main installation function
sub main {
    print "\n${BLUE}UNICS OS Development Environment Setup${NC}\n";
    print "${BLUE}=====================================${NC}\n\n";
    
    # Detect distribution
    my $distro = detect_distro();
    unless ($distro) {
        error("Unable to detect Linux distribution");
        exit(1);
    }
    
    info("Detected distribution: \u$distro");
    
    # Check if running as root
    if ($< == 0) {
        warning("Running as root is not recommended. Use sudo instead.");
    }
    
    # Confirm installation
    print "Proceed with dependency installation? [y/N] ";
    my $answer = <STDIN>;
    chomp $answer;
    
    unless ($answer =~ /^y(es)?$/i) {
        info("Installation cancelled");
        exit(0);
    }
    
    # Check sudo privileges
    unless ($< == 0 || command_exists("sudo")) {
        error("sudo is required but not available");
        exit(1);
    }
    
    # Install dependencies
    if (install_deps($distro)) {
        if (verify_installation()) {
            success("\nAll dependencies installed successfully!");
            print "\nYou can now compile Unics OS with:\n";
            print "  make clean\n";
            print "  make all\n";
        } else {
            error("\nInstallation completed but some components are missing");
            print "\nYou may need to manually install missing components\n";
            exit(1);
        }
    } else {
        error("Failed to install dependencies");
        exit(1);
    }
}

# Execute main function
main();
