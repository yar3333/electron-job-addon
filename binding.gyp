{
  "variables": {
    "napi_version": "8"
  },
  "targets": [
    {
      "target_name": "job_addon",
      "conditions": [
        ['OS=="win"', {
          "sources": [
            "src/addon.cc",
            "src/JobAddon.cc"
          ],
          "include_dirs": [
            "<!@(node -p \"require('node-addon-api').include\")",
            "include"
          ],
          "libraries": [
            "kernel32.lib"
          ],
          "dependencies": [
            "<!(node -p \"require('node-addon-api').gyp\")"
          ],
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": 1,
              "DebugInformationFormat": "OldStyle",
              "AdditionalOptions": ["/FS"]
            },
            "VCLinkerTool": {
              "GenerateDebugInformation": "true"
            }
          },
          "defines": [
            "NODE_ADDON_API_CPP_EXCEPTIONS",
            "NAPI_VERSION=8",
            "WINVER=0x0A00",
            "_WIN32_WINNT=0x0A00"
          ]
        }]
      ]
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "job_addon" ],
      "copies": [
        {
          "files": [
            "<(PRODUCT_DIR)/job_addon.node"
          ],
          "destination": "./bin/<(OS)-<(target_arch)-<(napi_version)"
        }
      ]
    }
  ]
}
