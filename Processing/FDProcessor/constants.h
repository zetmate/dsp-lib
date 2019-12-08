#pragma once

namespace fftLimits {
    const int minOrder = 7;
    const int maxOrder = 12;
    const int maxSize = 1 << maxOrder;
    const int maxDataSize = maxSize * 2; // Data should be twice as large as fft
    
    const int maxNumThreads = 8;
}

namespace fftDefault {
    const int order = 10;
    const int size = 1 << order;
    const int dataSize = size * 2; // Data should be twice as large as fft
    
    const int numThreads = 4;
}
