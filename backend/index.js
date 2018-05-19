const { spawn } = require('child_process');
const path = require('path');
const { unlink, rmdir, mkdir, watch, readFile } = require('mz/fs');

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
io.sockets.on('connection', (socket) => {
  console.log('socket connected');
  socket.emit('connected');
  socket.on('run', async (params) => {
    // create tmp folder
    const folder = path.resolve('algorithm', 'tmp');
    await mkdir(folder);
    // watch tmp folder
    let unlinkPms = [];
    const watcher = watch(folder, async (eventType, filename) => {
      if (eventType === 'change') {
        filename = path.resolve(folder, filename);
        let content = await readFile(filename);
        try {
          let data = JSON.parse(content.toString());
          socket.emit(data.type, data.data);
        } catch (err) {
          console.log(`Parse data error, reason: ${err}, data: ${content}`);
        }
        unlinkPms.push(unlink(filename));
      }
    });

    console.log(`Running algorithm with params ${JSON.stringify(params)}`);
    let algo = spawn('main', [params.func, params.D], {
      cwd: path.resolve(__dirname, 'algorithm')
    });
    algo.on('stdio', (content) => {
      console.log(content.toString());
    })
    algo.on('error', (err) => {
      console.log(`Run algorithm error, reason: ${err}`);
    });
    algo.on('exit', async (code) => {
      console.log(`Algorithm finished with exit code ${code}`);

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
  });
});

app.get('/', (req, res) => {
  res.send('ia backend working');
});

const PORT = 5000;
server.listen(PORT, () => {
  console.log(`Listening ${PORT}`);
});