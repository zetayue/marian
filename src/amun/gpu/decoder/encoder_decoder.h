#pragma once

#include <vector>
#include <yaml-cpp/yaml.h>

#include "common/scorer.h"
#include "common/base_best_hyps.h"
#include "common/threadpool.h"
#include "common/history.h"
#include "gpu/types-gpu.h"
#include "gpu/mblas/matrix.h"
#include "gpu/mblas/handles.h"
#include "enc_out_buffer.h"


namespace amunmt {
namespace GPU {

class EncoderDecoderState;
class Encoder;
class Decoder;
class Weights;
class BeamSizeGPU;

/////////////////////////////////////////////////////////////////////////
class EncoderDecoder : public Scorer {
  private:
    typedef EncoderDecoderState EDState;

  public:
    EncoderDecoder(const God &god,
                   const std::string& name,
                   const YAML::Node& config,
                   size_t tab,
                   const Weights& model,
                   const Search &search);

    virtual ~EncoderDecoder();

    virtual void Decode(const State& in, State& out, const BeamSize& beamSizes)
    {
      abort();
    }

    virtual void Decode(const State& in,
                        mblas::Matrix &nextStateMatrix,
                        const BeamSize& beamSizes);

    virtual State* NewState() const;


    virtual void Encode(const SentencesPtr source);

    virtual void AssembleBeamState(const State& in,
                                   const Hypotheses& hypos,
                                   State& out)
    {
      abort();
    }

    virtual void AssembleBeamState(const mblas::Matrix &nextStateMatrix,
                                   const Hypotheses& hypos,
                                   State& out);

    void GetAttention(mblas::Matrix& Attention);

    mblas::Matrix& GetAttention();
    virtual BaseMatrix& GetProbs();

    size_t GetVocabSize() const;

    void Filter(const std::vector<size_t>& filterIds);

  private:
    const Weights& model_;
    std::unique_ptr<Encoder> encoder_;
    std::unique_ptr<Decoder> decoder_;
    DeviceVector<uint> indices_;
      // set in Encoder::GetContext() to length (maxSentenceLength * batchSize). 1 if it's a word, 0 otherwise

    EncOutBuffer encDecBuffer_;
    std::unique_ptr<std::thread> decThread_;

    void DecodeAsync(const God &god);
    void DecodeAsyncInternal(const God &god);
    void BeginSentenceState(mblas::Matrix &states,
                            mblas::Matrix &embeddings,
                            BeamSizeGPU& beamSizes,
                            size_t batchSize,
                            const EncOut &encOut) const;

    EncoderDecoder(const EncoderDecoder&) = delete;
};

}
}

