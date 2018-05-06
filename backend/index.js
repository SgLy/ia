let { execFile } = require('mz/child_process');
let { unlink, rmdir, mkdir, watch, readFile } = require('mz/fs');
async function main() {
  // create tmp folder
  await mkdir('tmp');
  // watch tmp folder
  let unlinkPms = [];
  const watcher = watch('tmp', async (eventType, filename) => {
    if (eventType === 'change') {
      filename = `tmp/${filename}`;
      let content = await readFile(filename);
      console.log(JSON.parse(content.toString()));
      unlinkPms.push(unlink(filename));
    }
  });
  // run main task
  try {
    await execFile('main.exe');
  } catch (err) {
    console.log(`Running algorithm error, code: ${err.errno}`);
  }
  // wait all unlink and finish watch
  for (let i = 0; i < unlinkPms.length; ++i)
    await unlinkPms[i];
  watcher.close();
  // remove tmp folder
  try {
    await rmdir('tmp');
  } catch (err) {
    console.log(`Unable to remove tmp folder, error code: ${err.code}`);
  }
}

main();
