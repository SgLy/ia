const { spawn } = require('child_process');
const path = require('path');
const { unlink, rmdir, mkdir, watch, readFile } = require('mz/fs');
const { readdirSync } = require('fs');

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
  socket.on('stop', () => {
    if (socket.prog !== undefined)
      socket.prog.kill('SIGINT');
    socket.prog = undefined;
  });
  socket.on('run', async (params) => {
    if (socket.prog !== undefined)
      return ;
    // create tmp folder
    const folder = path.resolve('algorithm', 'tmp');
    try {
      await mkdir(folder);
    } catch(err) {}
    const pms = [];
    let count = 0;
    // watch tmp folder
    const watcher = watch(folder, (eventType, filename) => {
      if (eventType === 'change' && filename !== null)
        pms.push((async (eventType, filename) => {
          filename = path.resolve(folder, filename);
          let content = await readFile(filename);
          try {
            let data = JSON.parse(content.toString());
            socket.emit(data.type, data.data);
            ++count;
            return data.type;
          } catch (err) { return 'error'; }
        })(eventType, filename));
    });

    console.log(`Running algorithm with params ${JSON.stringify(params)}`);
    socket.prog = spawn('main', [params.func, params.D], {
      cwd: path.resolve(__dirname, 'algorithm')
    });
    socket.prog.on('stdio', (content) => {
      console.log(content.toString());
    })
    socket.prog.on('error', (err) => {
      console.log(`Run algorithm error, reason: ${err}`);
    });
    socket.prog.on('exit', async (code) => {
      console.log(`Algorithm finished with exit code ${code}`);
      socket.prog = undefined;

      setTimeout(async () => {
        for (let i = 0; i < pms.length; ++i)
          await pms[i];
        watcher.close();
        console.log(`Emit ${count} data`);
        // unlink all and finish watch
        await Promise.all(
          readdirSync(folder).map(f => unlink(path.resolve(__dirname, 'algorithm', 'tmp', f)))
        );

        // remove tmp folder
        try {
          await rmdir(folder);
        } catch (err) {
          console.log(`Unable to remove ${folder}, error code: ${err.code}`);
        }
        socket.emit('done');
      }, 200);
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