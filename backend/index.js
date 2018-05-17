let { execFile } = require('mz/child_process');
let { unlink, rmdir, mkdir, watch, readFile } = require('mz/fs');

const app = require('express')();
const cors = require('cors');
app.use(cors({
  origin: 'http://localhost:3000',
  credentials: true,
  optionsSuccessStatus: 204
}));

const server = require('http').Server(app);
const io = require('socket.io')(server);
app.io = io;
io.on('connection', () => {
  console.log('socket connected');
});
io.on('run', async (params) => {
  const folder = 'algorithm/tmp';
  console.log(folder);
  // create tmp folder
  await mkdir(folder);
  // watch tmp folder
  let unlinkPms = [];
  const watcher = watch(folder, async (eventType, filename) => {
    if (eventType === 'change') {
      filename = `${folder}/${filename}`;
      let content = await readFile(filename);
      io.emit('data', JSON.parse(content.toString()));
      unlinkPms.push(unlink(filename));
    }
  });
  // run main task
  try {
    await execFile('algorithm/main.exe');
  } catch (err) {
    console.log(`Running algorithm error, code: ${err.errno}`);
  }
  // wait all unlink and finish watch
  for (let i = 0; i < unlinkPms.length; ++i)
    await unlinkPms[i];
  watcher.close();
  // remove tmp folder
  try {
    await rmdir(folder);
  } catch (err) {
    console.log(`Unable to remove ${folder}, error code: ${err.code}`);
  }
});

app.get('/', (req, res) => {
  res.send('ia backend working');
});

const PORT = 5000;
server.listen(PORT, () => {
  console.log(`Listening ${PORT}`);
});