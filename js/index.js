const path = require("path");

/**
 * JavaScript wrapper for native addon Windows Job Objects
 *
 * Usage in Electron main process:
 *
 * const { Job } = require('./js/index.js');
 *
 * // Create Job
 * const job = new Job();
 * job.create('MyJob');
 *
 * // Add process by PID
 * job.addProcessById(pid);
 *
 * // Kill processes when Job closes
 * job.setKillOnJobClose(true);
 *
 * // Close Job
 * job.close();
 */

class Job {
  constructor() {
    if (process.platform !== "win32") {
      throw new Error("Job Objects are only available on Windows");
    }

    // Load native addon
    const native = require("bindings")("job_addon");
    this._addon = new native.Job();
  }

  /**
   * Creates Job Object
   * @param {string} [name] - Job Object name (optional)
   * @returns {boolean} true on success
   */
  create(name) {
    return this._addon.create(name);
  }

  /**
   * Closes Job Object
   * @returns {boolean} true on success
   */
  close() {
    return this._addon.close();
  }

  /**
   * Checks if Job Object is valid
   * @returns {boolean} true if Job Object is valid
   */
  isValid() {
    return this._addon.isValid();
  }

  /**
   * Adds a process to Job Object
   * @param {number|Buffer} handle - Process PID or Buffer with HANDLE
   * @returns {boolean} true on success
   */
  addProcess(handle) {
    return this._addon.addProcess(handle);
  }

  /**
   * Adds a process to Job Object by PID
   * @param {number} pid - Process identifier
   * @returns {boolean} true on success
   */
  addProcessById(pid) {
    return this._addon.addProcessById(pid);
  }

  /**
   * Sets kill on job close flag
   * @param {boolean} enable - true to enable
   * @returns {boolean} true on success
   */
  setKillOnJobClose(enable) {
    return this._addon.setKillOnJobClose(enable);
  }

  /**
   * Sets silent breakaway flag
   * @param {boolean} enable - true to enable
   * @returns {boolean} true on success
   */
  setSilentBreakaway(enable) {
    return this._addon.setSilentBreakaway(enable);
  }

  /**
   * Sets limit flags
   * @param {number} flags - Limit flags (bitmask)
   * @returns {boolean} true on success
   */
  setLimitFlags(flags) {
    return this._addon.setLimitFlags(flags);
  }
}

// Export constants from native addon
const native = process.platform === "win32" ? require("bindings")("job_addon") : null;

module.exports = {
  Job,
  JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE: native?.JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE || 0x00000001,
  JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK: native?.JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK || 0x00000002,
  JOB_OBJECT_LIMIT_BREAKAWAY_OK: native?.JOB_OBJECT_LIMIT_BREAKAWAY_OK || 0x00000800,
  JOB_OBJECT_LIMIT_PROCESS_MEMORY: native?.JOB_OBJECT_LIMIT_PROCESS_MEMORY || 0x00000100,
  JOB_OBJECT_LIMIT_JOB_MEMORY: native?.JOB_OBJECT_LIMIT_JOB_MEMORY || 0x00000200,
  JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION: native?.JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION || 0x00000400,
  JOB_OBJECT_LIMIT_ACTIVE_PROCESS: native?.JOB_OBJECT_LIMIT_ACTIVE_PROCESS || 0x00002000,
};
