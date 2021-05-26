#ifndef ROUTING_PREPROCESSING_ALGORITHM_PREPROCESSOR_H
#define ROUTING_PREPROCESSING_ALGORITHM_PREPROCESSOR_H

#include "routing/profile/profile.h"

namespace routing{
namespace preprocessing{

class AlgorithmPreprocessor{
public:
    AlgorithmPreprocessor() {}

    virtual void RunPreprocessing(profile::Profile& profile) = 0;

    AlgorithmPreprocessor(const AlgorithmPreprocessor& other) = delete;
    AlgorithmPreprocessor(AlgorithmPreprocessor&& other) = delete;
    AlgorithmPreprocessor& operator=(const AlgorithmPreprocessor& other) = delete;
    AlgorithmPreprocessor& operator=(AlgorithmPreprocessor&& other) = delete;
    virtual ~AlgorithmPreprocessor() {}
};




}
}

#endif // ROUTING_PREPROCESSING_ALGORITHM_PREPROCESSOR_H