/**
 * Пример использования Job Objects в Electron main process
 * 
 * Этот файл демонстрирует, как использовать native addon для управления
 * Windows Job Objects из Electron приложения.
 */

const { spawn } = require('child_process');
const { Job, JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE } = require('./js/index.js');

// ============================================================================
// Пример 1: Базовое использование
// ============================================================================

function basicExample() {
  console.log('=== Базовый пример ===');

  // Создаём Job
  const job = new Job();
  job.create('MyTestJob');

  // Запускаем процесс (например, notepad)
  const child = spawn('notepad.exe');
  console.log(`Запущен процесс с PID: ${child.pid}`);

  // Добавляем процесс к Job
  const result = job.addProcessById(child.pid);
  console.log(`Процесс добавлен к Job: ${result}`);

  // Устанавливаем флаг: убить процесс при закрытии Job
  job.setKillOnJobClose(true);

  // Закрываем Job (процесс будет убит)
  job.close();
}

// ============================================================================
// Пример 2: Управление несколькими процессами
// ============================================================================

function multipleProcessesExample() {
  console.log('=== Несколько процессов ===');

  const job = new Job();
  job.create();

  // Запускаем несколько процессов
  const processes = [];
  for (let i = 0; i < 3; i++) {
    const child = spawn('notepad.exe');
    processes.push(child);
    console.log(`Запущен процесс ${i + 1} с PID: ${child.pid}`);
    
    // Добавляем каждый процесс к Job
    job.addProcessById(child.pid);
  }

  // Все процессы будут убиты при закрытии Job
  job.setKillOnJobClose(true);

  // Ждём 5 секунд и закрываем Job
  setTimeout(() => {
    console.log('Закрываем Job, все процессы будут убиты...');
    job.close();
  }, 5000);
}

// ============================================================================
// Пример 3: Использование с флагами
// ============================================================================

function flagsExample() {
  console.log('=== Пример с флагами ===');

  const job = new Job();
  job.create('MyJobWithFlags');

  // Комбинируем флаги
  const flags = 
    JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE |
    0x00000400; // JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION

  job.setLimitFlags(flags);

  const child = spawn('notepad.exe');
  job.addProcessById(child.pid);

  console.log(`Процесс ${child.pid} добавлен к Job с флагами`);
}

// ============================================================================
// Пример 4: Интеграция с Electron
// ============================================================================

/**
 * В реальном Electron приложении вы можете использовать Job для управления
 * дочерними процессами, например, для запуска внешних утилит.
 */
function electronIntegrationExample() {
  const { app, BrowserWindow } = require('electron');

  let mainWindow;
  let job;
  let workerProcess;

  function createWindow() {
    mainWindow = new BrowserWindow({
      width: 800,
      height: 600,
      webPreferences: {
        nodeIntegration: true,
        contextIsolation: false
      }
    });

    mainWindow.loadFile('index.html');

    // Создаём Job для управления рабочим процессом
    job = new Job();
    job.create('ElectronWorkerJob');
    job.setKillOnJobClose(true);

    // Запускаем рабочий процесс
    workerProcess = spawn('your-worker.exe', ['--arg1', '--arg2']);
    
    // Добавляем рабочий процесс к Job
    job.addProcessById(workerProcess.pid);

    mainWindow.on('closed', () => {
      mainWindow = null;
      // При закрытии окна Job автоматически закроется
      // и рабочий процесс будет убит (из-за флага KillOnJobClose)
      job.close();
    });
  }

  app.whenReady().then(createWindow);

  app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
      app.quit();
    }
  });
}

// ============================================================================
// Запуск примеров
// ============================================================================

// Раскомментируйте нужный пример:
// basicExample();
// multipleProcessesExample();
// flagsExample();

module.exports = { basicExample, multipleProcessesExample, flagsExample };
