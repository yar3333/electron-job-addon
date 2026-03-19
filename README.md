# Electron Job Addon

Native addon for working with Windows Job Objects from Electron main process.

## 📁 Project Structure

```
ElectronJobs/
├── binding.gyp              # node-gyp build configuration
├── package.json             # Dependencies and scripts
├── include/
│   └── JobAddon.h           # C++ header file
├── src/
│   ├── JobAddon.cc          # Job Object implementation
│   └── addon.cc             # Node-API wrapper
├── js/
│   ├── index.js             # JavaScript wrapper
│   └── index.d.ts           # TypeScript definitions
└── examples/
    └── usage.js             # Usage examples
```

## 🚀 Installation and Build

```bash
# Install dependencies
npm install

# Build native addon
npm run build

# Rebuild (clean + build)
npm run rebuild
```

## ✅ Testing

```bash
# Run tests
node test.js
```

## 📖 Usage

### Basic Example

```javascript
const { Job, JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE } = require("./js/index.js");

// Create Job
const job = new Job();
job.create("MyJob");

// Spawn a process
const { spawn } = require("child_process");
const child = spawn("notepad.exe");

// Add process to Job
job.addProcessById(child.pid);

// Set flag: kill process when Job closes
job.setKillOnJobClose(true);

// Close Job (process will be killed)
job.close();
```

### Managing Multiple Processes

```javascript
const job = new Job();
job.create();
job.setKillOnJobClose(true);

// Spawn multiple processes
const p1 = spawn("process1.exe");
const p2 = spawn("process2.exe");
const p3 = spawn("process3.exe");

job.addProcessById(p1.pid);
job.addProcessById(p2.pid);
job.addProcessById(p3.pid);

// All 3 processes will be killed when Job closes
job.close();
```

### Using with Flags

```javascript
const {
  Job,
  JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE,
  JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION,
} = require("./js/index.js");

const job = new Job();
job.create();

// Combine flags
const flags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;

job.setLimitFlags(flags);
```

## 📋 API

### `Job` Class

#### `constructor()`

Creates a Job wrapper instance.

#### `create(name?: string): boolean`

Creates a Windows Job Object.

- `name` - optional Job Object name

#### `close(): boolean`

Closes the Job Object.

#### `isValid(): boolean`

Checks if the Job Object is valid.

#### `addProcess(handle: number | Buffer): boolean`

Adds a process to the Job Object.

- `handle` - process PID (number) or HANDLE (Buffer)

#### `addProcessById(pid: number): boolean`

Adds a process to the Job Object by PID.

#### `setKillOnJobClose(enable: boolean): boolean`

Sets the flag to kill processes when Job closes.

#### `setSilentBreakaway(enable: boolean): boolean`

Sets the silent breakaway flag.

#### `setLimitFlags(flags: number): boolean`

Sets limit flags (bitmask).

### Constants

| Constant                                      | Value        | Description                      |
| --------------------------------------------- | ------------ | -------------------------------- |
| `JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE`          | `0x00000001` | Kill processes when Job closes   |
| `JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK`        | `0x00000002` | Allow silent breakaway           |
| `JOB_OBJECT_LIMIT_BREAKAWAY_OK`               | `0x00000800` | Allow breakaway from Job         |
| `JOB_OBJECT_LIMIT_PROCESS_MEMORY`             | `0x00000100` | Process memory limit             |
| `JOB_OBJECT_LIMIT_JOB_MEMORY`                 | `0x00000200` | Job memory limit                 |
| `JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION` | `0x00000400` | Terminate on unhandled exception |
| `JOB_OBJECT_LIMIT_ACTIVE_PROCESS`             | `0x00002000` | Active process limit             |

## 🔧 Requirements

- Windows 10 or higher
- Node.js 16+
- Electron 15+
- node-gyp
- Visual Studio Build Tools (C++)

## 📝 Notes

1. **Windows Only**: Job Objects is a Windows technology, the addon will not work on other platforms.

2. **Run as Administrator**: Some Job Objects operations may require administrator privileges.

3. **Electron Integration**: After building, you need to rebuild the addon for your Electron version:
   ```bash
   set PYTHON=c:\Server\python-3.11\python.exe && npx electron-rebuild -v 41.0.3
   ```

## 🔗 Links

- [Microsoft Documentation: Job Objects](https://docs.microsoft.com/en-us/windows/win32/procthread/job-objects)
- [Electron: Native Code and Win32](https://www.electronjs.org/docs/latest/tutorial/native-code-and-electron-cpp-win32)
- [node-addon-api](https://github.com/nodejs/node-addon-api)
