// Suppress codecvt deprecation warning in C++17
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "apfs/container.h"
#include "apfs/volume.h"
#include "winfsp/winfsp_ops.h"
#include "utils/disk_reader.h"
#include "utils/logging.h"
#include <iostream>
#include <string>
#include <memory>
#include <csignal>
#include <locale>
#include <codecvt>
#include <windows.h>

static WinFspAPFS* g_mounted_fs = nullptr;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nReceived interrupt signal, unmounting..." << std::endl;
        if (g_mounted_fs) {
            g_mounted_fs->unmount();
        }
        exit(0);
    }
}

void printUsage(const char* program_name) {
    std::cout << "WAMount - Windows Apple Mount (APFS Reader)\n";
    std::cout << "Version 1.0\n\n";
    std::cout << "Usage: " << program_name << " [options] -m <mountpoint> <device>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -m <drive>      Mount point (e.g., X:)\n";
    std::cout << "  -v <index>      Volume index (default: 0)\n";
    std::cout << "  -d              Enable debug output\n";
    std::cout << "  -h              Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " -m X: \\\\.\\PhysicalDrive1\n";
    std::cout << "  " << program_name << " -m X: -v 0 \\\\.\\PhysicalDrive1\n";
    std::cout << "  " << program_name << " -d -m X: \\\\.\\PhysicalDrive1\n\n";
    std::cout << "Notes:\n";
    std::cout << "  - Requires WinFsp to be installed (https://winfsp.dev/)\n";
    std::cout << "  - Run with administrator privileges\n";
    std::cout << "  - Press Ctrl+C to unmount and exit\n";
}

int main(int argc, char* argv[]) {
    std::string device_path;
    std::string mount_point;
    int volume_index = 0;
    bool debug = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-d" || arg == "--debug") {
            debug = true;
        } else if (arg == "-m" || arg == "--mount") {
            if (i + 1 < argc) {
                mount_point = argv[++i];
            } else {
                std::cerr << "Error: -m requires a mount point\n";
                return 1;
            }
        } else if (arg == "-v" || arg == "--volume") {
            if (i + 1 < argc) {
                volume_index = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: -v requires a volume index\n";
                return 1;
            }
        } else if (arg[0] != '-') {
            device_path = arg;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // Validate arguments
    if (device_path.empty() || mount_point.empty()) {
        std::cerr << "Error: Device path and mount point are required\n\n";
        printUsage(argv[0]);
        return 1;
    }

    // Set logging level
    if (debug) {
        Logger::setLevel(LogLevel::DEBUG);
    }

    // Register signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::cout << "WAMount - Windows Apple Mount v1.0\n";
    std::cout << "==================================\n\n";

    // Open disk
    auto disk = std::make_shared<DiskReader>();
    if (!disk->open(device_path)) {
        std::cerr << "Failed to open device: " << device_path << "\n";
        std::cerr << "Make sure you're running with administrator privileges.\n";
        return 1;
    }

    // Initialize APFS container
    auto container = std::make_shared<APFSContainer>(disk);
    if (!container->init()) {
        std::cerr << "Failed to initialize APFS container\n";
        std::cerr << "This may not be a valid APFS partition.\n";
        return 1;
    }

    std::cout << "Container info:\n";
    std::cout << "  Block size: " << container->getBlockSize() << " bytes\n";
    std::cout << "  Total blocks: " << container->getBlockCount() << "\n";
    std::cout << "  Volumes found: " << container->getVolumeCount() << "\n\n";

    // Get volume
    auto volume = container->getVolume(volume_index);
    if (!volume) {
        std::cerr << "Failed to open volume " << volume_index << "\n";
        return 1;
    }

    std::cout << "Volume info:\n";
    std::cout << "  Name: " << volume->getVolumeName() << "\n";
    std::cout << "  Files: " << volume->getSuperblock()->apfs_num_files << "\n";
    std::cout << "  Directories: " << volume->getSuperblock()->apfs_num_directories << "\n\n";

    // Mount with WinFsp
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring mount_point_wide = converter.from_bytes(mount_point);

    auto winfsp_fs = std::make_unique<WinFspAPFS>(volume);
    g_mounted_fs = winfsp_fs.get();

    if (!winfsp_fs->mount(mount_point_wide)) {
        std::cerr << "Failed to mount volume\n";
        std::cerr << "Make sure WinFsp is installed and the mount point is available.\n";
        return 1;
    }

    std::cout << "Volume mounted successfully at " << mount_point << "\n";
    std::cout << "Press Ctrl+C to unmount and exit...\n\n";

    // Wait for signal
    while (true) {
        Sleep(1000);
    }

    return 0;
}
