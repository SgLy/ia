let { execFile } = require('child_process');
async function run(filename) {
  return new Promise((resolve, reject) => {
    execFile(filename, function (err, data) {
      if (err)
        reject(err);
      else
        resolve(data);
    });
  });
}

let { unlink, rmdir, mkdir, watch, readFile } = require('mz/fs');
async function main() {
  // create tmp folder
  await mkdir('tmp');
  // watch tmp folder
  const watcher = watch('tmp', async (eventType, filename) => {
    if (eventType === 'change') {
      filename = `tmp/${filename}`;
      let content = await readFile(filename);
      console.log(JSON.parse(content.toString()));
      unlink(filename);
    }
  });
  // run main task
  try {
    await run('main.exe');
  } catch (err) {
    console.log(`Running algorithm error, code: ${err.errno}`);
  }
  // finish watch
  watcher.close();
  // remove tmp folder
  try {
    await rmdir('tmp');
  } catch (err) {
    console.log(`Unable to remove tmp folder, error code: ${err.code}`);
  }
}

main();
