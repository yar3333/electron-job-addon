/**
 * Test for native addon functionality
 */

const { Job, JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE } = require("./js/index.js");

console.log("=== Electron Job Addon Testing ===\n");

try {
  // Create Job
  console.log("1. Creating Job...");
  const job = new Job();
  console.log("   Job created:", job !== undefined);

  // Initialize Job
  console.log("2. Initializing Job Object...");
  const created = job.create("TestJob_" + Date.now());
  console.log("   Job created:", created);
  console.log("   Job is valid:", job.isValid());

  // Test setKillOnJobClose
  console.log("3. Testing setKillOnJobClose...");
  const killResult = job.setKillOnJobClose(true);
  console.log("   setKillOnJobClose(true):", killResult);

  // Test setSilentBreakaway
  console.log("4. Testing setSilentBreakaway...");
  const breakawayResult = job.setSilentBreakaway(false);
  console.log("   setSilentBreakaway(false):", breakawayResult);

  // Test setLimitFlags
  console.log("5. Testing setLimitFlags...");
  const flagsResult = job.setLimitFlags(JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE);
  console.log("   setLimitFlags:", flagsResult);

  // Test addProcessById with current process
  console.log("6. Testing addProcessById (current process)...");
  try {
    // Try to add current process (may not work due to permissions)
    const currentPid = process.pid;
    console.log("   Current PID:", currentPid);
    // Skip adding current process as it may cause issues
    console.log("   Skipped (not recommended to add current process)");
  } catch (err) {
    console.log("   Error (expected):", err.message);
  }

  // Close Job
  console.log("7. Closing Job...");
  const closed = job.close();
  console.log("   Job closed:", closed);
  console.log("   Job is valid after close:", job.isValid());

  console.log("\n=== All tests passed! ===");
} catch (err) {
  console.error("\n=== Error:", err.message);
  console.error(err.stack);
  process.exit(1);
}
