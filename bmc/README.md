# Format of Config file

This document gives details about the format of the config file useed by log, version and firmware handler. The config file is a .json file.

### Parameters

There are 3 important parameters in this config file
1. blob
2. handler
3. actions

An example config file -

```
{
    "blob": "/flash/adm1266_sink0",
    "handler": {
        "type": "file",
        "path": "/var/run/adm1266/adm1266_sink0.hex"
    },
    "actions": {
        "preparation": {
            "type": "skip"
        },
        "verification": {
            "type": "systemd",
            "unit": "adm1266-verify@sink0.service"
        },
        "update": {
            "type": "systemd",
            "unit": "adm1266-update@sink0.service"
        }
    }
}
```

#### blob
This parameter defines the unique name of the blob. This parameter must be defined for each blob.

#### handler
A blob must have a handler with a type. Currently only "file" type is supported. With file type, a path of the handler file must be provided.

#### actions
"actions" define various steps to be performed and are required for each blob. These actions can be triggered from ipmi-blob commands. Currently there are 2 types of actions supported: `systemd` to invoke a systemd service, or `skip` to not perform any action.