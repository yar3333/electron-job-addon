/**
 * TypeScript definitions for native addon Windows Job Objects
 */

export interface JobOptions {
  /** Job Object name (optional) */
  name?: string;
  /** Kill processes when Job closes */
  killOnJobClose?: boolean;
  /** Allow silent breakaway from Job */
  silentBreakaway?: boolean;
}

export declare class Job {
  constructor();

  /**
   * Creates Job Object
   * @param name - Job Object name (optional)
   * @returns true on success
   */
  create(name?: string): boolean;

  /**
   * Closes Job Object
   * @returns true on success
   */
  close(): boolean;

  /**
   * Checks if Job Object is valid
   * @returns true if Job Object is valid
   */
  isValid(): boolean;

  /**
   * Adds a process to Job Object
   * @param handle - Process PID or Buffer with HANDLE
   * @returns true on success
   */
  addProcess(handle: number | Buffer): boolean;

  /**
   * Adds a process to Job Object by PID
   * @param pid - Process identifier
   * @returns true on success
   */
  addProcessById(pid: number): boolean;

  /**
   * Sets kill on job close flag
   * @param enable - true to enable
   * @returns true on success
   */
  setKillOnJobClose(enable: boolean): boolean;

  /**
   * Sets silent breakaway flag
   * @param enable - true to enable
   * @returns true on success
   */
  setSilentBreakaway(enable: boolean): boolean;

  /**
   * Sets limit flags
   * @param flags - Limit flags (bitmask)
   * @returns true on success
   */
  setLimitFlags(flags: number): boolean;
}

// Limit flag constants
export declare const JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE: number;
export declare const JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK: number;
export declare const JOB_OBJECT_LIMIT_BREAKAWAY_OK: number;
export declare const JOB_OBJECT_LIMIT_PROCESS_MEMORY: number;
export declare const JOB_OBJECT_LIMIT_JOB_MEMORY: number;
export declare const JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION: number;
export declare const JOB_OBJECT_LIMIT_ACTIVE_PROCESS: number;

// Default export
export declare const jobAddon: {
  Job: typeof Job;
  JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE: number;
  JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK: number;
  JOB_OBJECT_LIMIT_BREAKAWAY_OK: number;
  JOB_OBJECT_LIMIT_PROCESS_MEMORY: number;
  JOB_OBJECT_LIMIT_JOB_MEMORY: number;
  JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION: number;
  JOB_OBJECT_LIMIT_ACTIVE_PROCESS: number;
};
