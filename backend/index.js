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
  // clean and create tmp folder
  try {
    await rmdir('tmp');
  } catch (err) {
    if (err.code !== 'ENOTEMPTY' && err.code !== 'ENOENT')
      throw(err);
  }
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
  try {
    await run('main.exe');
    watcher.close();
  } catch (err) {
    console.log(err.errno);
  }
}

main();
