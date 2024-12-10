#include "../include/initialization.cuh"
#include <fstream>
#include <nlohmann/json.hpp>

void initializeBodies(Bodies bodies, int numberOfBodies, double spreadX,
                        double spreadY, double spreadZ) {
    double mass = 10e15;

  for (int i = 0; i < numberOfBodies; i++) {
    // initialize the bodies randomly spread around 0,0
    double x = static_cast<double>(rand()) / static_cast<double>(RAND_MAX) - 0.5;
    double y = static_cast<double>(rand()) / static_cast<double>(RAND_MAX) - 0.5;
    double z = static_cast<double>(rand()) / static_cast<double>(RAND_MAX) - 0.5;
    bodies.position[i].x = x * spreadX;
    bodies.position[i].y = x * spreadY;
    bodies.position[i].z = x * spreadZ;

    bodies.velocity[i].x = 0;
    bodies.velocity[i].y = 0;
    bodies.velocity[i].z = 0;

    bodies.mass[i] = mass;

    bodies.acceleration[i].x = 0;
    bodies.acceleration[i].y = 0;
    bodies.acceleration[i].z = 0;
  }
}


using json = nlohmann::json;
Config parseConfig(const int argc, const char** argv) {
  Config config;

  if (argc <= 1) {
    return config;
  }

  if (argc > 1) {
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
      std::cout << "Usage: " << argv[0] << " [number of bodies] [iterations] [save interval] [dt] [output filename]" << std::endl;
      exit(0);
    } else if (strcmp(argv[1], "--config") == 0 || strcmp(argv[1], "-c") == 0) {
      std::ifstream file(argv[2]);
      if (file.is_open()) {
        json configJson;
        file >> configJson;
        file.close();

        config.numberOfBodies = configJson["numberOfBodies"];
        config.iterations = configJson["iterations"];
        config.saveInterval = configJson["saveInterval"];
        config.dt = configJson["dt"];
        config.outputFilename = configJson["outputFilename"];
      } else {
        std::cout << "Error opening file." << std::endl;
        exit(0);
      }
    } else {
      config.numberOfBodies = atoi(argv[1]);
    }

    if (argc > 1) {
      config.numberOfBodies = atoi(argv[1]);
    }

    if (argc > 2) {
      config.iterations = atoi(argv[2]);
    }

    if (argc > 3) {
      config.saveInterval = atoi(argv[3]);
    }

    if (argc > 4) {
      config.dt = atof(argv[4]);
    }

    if (argc > 5) {
      config.outputFilename = argv[5];
    }
  }

  return config;
}

int getCudaBlockSize() {
  int deviceCount;
  cudaError_t err = cudaGetDeviceCount(&deviceCount);

  if (err != cudaSuccess) {
    std::cerr << "Error getting device count: " << cudaGetErrorString(err) << std::endl;
    exit(1);
  }

  if (deviceCount == 0) {
    std::cerr << "No CUDA devices found" << std::endl;
    exit(1);
  } else {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    std::cout << "Found CUDA device: " << prop.name << std::endl;
    std::cout << "Block size: " << prop.maxThreadsPerBlock << std::endl;
    return prop.maxThreadsPerBlock;
  }
}