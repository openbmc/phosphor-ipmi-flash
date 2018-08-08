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

    /**
     * Instantiate an UploadManager if the parameters check out.
     *
     * @param[in] image - path to the firmware image.
     * @param[in] hash - path to the image's hash.
     * @param[in] helper - pointer to an UpdateHelperInterface.
     * @param[in] dintf - pointer to the data interface to use.
     * @return UploadManager if valid or nullptr if invalid.
     */
    static std::unique_ptr<UploadManager>
        BuildUploadMgr(const std::string &image, const std::string &hash,
                       UpdateHelperInterface *helper, DataInterface *dintf);

    /**
     * Try to update the BMC flash image over IPMI through BT.
     */
    void UpdateBMC();

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
