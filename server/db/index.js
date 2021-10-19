const logger = require("../logger");
const sqlite3 = require('sqlite3').verbose();

let db = new sqlite3.Database('./db/caelum.db', sqlite3.OPEN_READWRITE, (err) => {
    if (err) {
      console.error(err.message);
    }
    logger.info('Connected to the CAELUM database.');
  });

function query(sql) {
    return new Promise(function(resolve, reject) {
        db.all(sql, [], (err, rows) => {
            if (err) {
                reject({result:'ERROR', msg: err});
            } else {
                resolve({result:'SUCCESS', rows: rows});
            }
        });
    });
}

function insert(table, columns, values) {
    return new Promise(function(resolve, reject) {
        logger.info('=========================================================')
        logger.info(`table: "${table}" columns: [${columns}] values: [${values}]`);
        logger.info('=========================================================')
        logger.info(`INSERT INTO ${table} (${columns}) VALUES(${Array(values.length).fill('?')})`);
        logger.info('=========================================================')
        db.run(`INSERT INTO ${table} (${columns}) VALUES(${Array(values.length).fill('?')})`, values, function(err) {
            if (err) {
                console.error(`ERROR: ${err}`);
                reject({result:'ERROR', msg: err});
            } else {
                // get the last insert id
                logger.info(`A row has been inserted with rowid ${this.lastID}`);
                resolve({result:'SUCCESS', table: table, columns: columns, values: values});
            }            
        });
    });
}

function close() {
    return new Promise(function(resolve, reject) {
        db.close((err) => {
            if (err) {
                reject({result:'ERROR', msg: err});
            } else {
                resolve({result:'SUCCESS', msg: `DB CONNECTION CLOSED`});
            }
        });
    });
}

module.exports = {
  query: (sql) => {
    return query(sql);
  },
  insert: (table, columns, values) => {
    return insert(table, columns, values);
  },
  close: () => {
    return close();
  }
};