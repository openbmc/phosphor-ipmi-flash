#pragma once

#include <fstream>
#include <memory>
#include <string>

#include "interface.hpp"
#include "updatehelper.hpp"

class UploadManager
{
  public:
    UploadManager(std::ifstream &&image, std::ifstream &&hash,
                  int32_t imageSize, int32_t hashSize,
                  UpdateHelperInterface *helper, DataInterface *dintf) :
        imageStream(std::move(image)),
        hashStream(std::move(hash)), imageSize(imageSize), hashSize(hashSize),
        helper(helper), dintf(dintf)
    {
    }

    static std::unique_ptr<UploadManager>
        BuildUploadMgr(const std::string &image, const std::string &hash,
                       UpdateHelperInterface *helper, DataInterface *dintf);

  private:
    std::ifstream imageStream;
    std::ifstream hashStream;
    int32_t imageSize;
    int32_t hashSize;
    UpdateHelperInterface *helper;
    DataInterface *dintf;
};

// Main entry point for the update command.
// Update uploads and verifies the image.
// throws exception on errors.
void UpdaterMain(const std::string &interface, const std::string &image,
                 const std::string &signature);
