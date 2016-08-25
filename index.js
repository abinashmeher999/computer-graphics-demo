express = require('express');
bodyParser = require('body-parser');
methodOverride = require('method-override');

// Create the application.
var app = express();
app.set('port', (process.env.PORT || 3000));

// Add Middleware necessary for REST API's
app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());
app.use(methodOverride('X-HTTP-Method-Override'));

// CORS Support
app.use(function(req, res, next) {
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE');
    res.header('Access-Control-Allow-Headers', 'Content-Type');
    next();
});

app.use(express.static('public'));

function sh(cmd) {
    var exec = require('child_process').execSync;

    return exec(cmd, function (error, stdout, stderr) {
        console.log(stdout);
        console.error(stderr);
        if (error !== null) {
            console.error(new Error(error));
        }
    });
}

{
    app.get('/', function(req, res) {
        res.sendFile(path.join(__dirname, "../public", "index.html"));
    });

    app.get('/3ddss/:x1/:y1/:z1/:x2/:y2/:z2', function(req, res) {
        //call the script
        first_coord = req.params.x1 + " " + req.params.y1 + " " + req.params.z1;
        second_coord = req.params.x2 + " " + req.params.y2 + " " + req.params.z2;
        res.send(sh("./3ddss " + first_coord + " " + second_coord));
    });

    app.get('/3ddtr/:x1/:y1/:z1/:x2/:y2/:z2/:x3/:y3/:z3', function(req, res) {
        first_coord = req.params.x1 + " " + req.params.y1 + " " + req.params.z1;
        second_coord = req.params.x2 + " " + req.params.y2 + " " + req.params.z2;
        third_coord = req.params.x3 + " " + req.params.y3 + " " + req.params.z3;
        res.send(sh("./3ddtr " + first_coord + " " + second_coord + " " + third_coord));
    });

    console.log('Listening on port...' + app.get('port'));
    app.listen(app.get('port'),'0.0.0.0');
};
