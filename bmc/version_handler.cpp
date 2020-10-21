/**
 * factory to create a BlobHandler for use by phosphor-ipmi-blobs
 */
std::unique_ptr<blobs::GenericBlobInterface>
    VersionBlobHandler::CreateFirmwareBlobHandler(
        std::vector<VersionPack>&& firmwares,ActionMap&& actionPacks)
{
  if(firmwares.empty())
  {
    return nullptr;
  }
  if(actionPacks.empty())
  {
    return nullptr;
  }
}

bool VersionBlobHandler::canHandleBlob(const std::string& path)
{
    return (std::find(blobIDs.begin(), blobIDs.end(), path) != blobIDs.end());
}

std::vector<std::string> VersionBlobHandler::getBlobIds()
{
  return blobIDs;
}

/**
 * deleteBlob - does nothing, always fails
 */
bool VersionBlobHandler::deleteBlob(const std::string& path)
{
  return false;
}

/**
 * Return meta-data about a blob
 * typically called when the blob is not opened
 * in the meta data simply return the state of the blob
 */
bool VersionBlobHandler::stat(const std::string& path, BlobMeta* meta)
{
  //If nothing is open then nothing should be started
  meta->blobState = _blobState;
}

/**
 * On open fire off the Preparation action pack
 */
bool VersionBlobHandler::open(uint16_t session, uint16_t flags,
                               const std::string& path)
{
  /* VersionBlobs only support read */
  if(flags != blobs::read)
  {
    return false;
  }
  /** only one blob allowed open at a time */
  if(!_activeBlobId.empty())
  {
    return false;
  }
  if (!canHandleBlob(path))
  {
    return false;
  }
  _activeBlobId = path;
  _blobState |= blobs::open_read;
  //trigger the FileOpenAction
  return true;
}

/**
 * read back the version information from file
 */
std::vector<uint8_t> VersionBlobHandler::read(uint16_t session, uint32_t offset,
                                uint32_t requestedSize)
{
  /* read from file */
}

/**
 * write isn't implemented for verison blobs
 */
bool VersionBlobHandler::write(uint16_t session, uint32_t offset,
                                const std::vector<uint8_t>& data)
{
}


/**
 * write meta isn't implemented for verison blobs
 */
bool VersionBlobHandler::writeMeta(uint16_t session, uint32_t offset,
                                    const std::vector<uint8_t>& data)
{
  return false; //not supported
}

/**
 * commit does nothing
 */
bool VersionBlobHandler::commit(uint16_t session,
                                 const std::vector<uint8_t>& data)
{
  return false; //not supported
}


/**
 * On close, clean-up resources
 */
bool VersionBlobHandler::close(uint16_t session)
{
  //TODO: Check to see if session is valid
  return abortProcess();
}

// this is expected to be called on an already opened blob
// VersionStatus::NotYetStarted //the process of getting the version is not yet
// started
// VersionStatus::Started //version retrieval started
// VersionStatus::Available version information is available
// VersionStatus::Unavailable version information could not be retrieved
bool VersionBlobHandler::stat(uint16_t session, blobs::BlobMeta* meta)
{
  auto item = lookup.find(session);
  if(item == lookup.end())
  {
    return false;
  }
  //give some indication on the progress of retrieving version information from
  //the firmware. Is it done? Still in progress?
  getActionStatus();
}

bool VersionBlobHandler::expire(uint16_t session)
{
  return abortProcess();
}

bool VersionBlobHandler::abortProcess()
{
  _blobState = 0;
  // abort the version reading process and clean-up
  return true;
}
