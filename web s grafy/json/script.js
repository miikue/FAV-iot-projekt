<script>
    chartIt()

    const datumydoma = [];
    const tepotadoma = [];
    const vlhkostdoma = [];
    async function chartIt(){
    await getData();
    const ctx = document.getElementById('doma');
    new Chart(ctx, {
    type: 'line',
    data: {
    labels: datumydoma,
    datasets: [{
    label: 'teplota',
    data: tepotadoma,
    borderColor: "red",
    borderWidth: 1
},{
    label: 'vlhkost',
    data: vlhkostdoma,
    borderColor: "blue",
    borderWidth: 1
}]
},
    options: {
    animation: false,
    parsing: false,
    interaction: {
    mode: 'nearest',
    axis: 'x',
    intersect: false
},
    title: {
    display: true,
    text: "Doma"
},
    plugins: {
    decimation: {
    enabled: true,
    algorithm: 'min-max',
    threshold: 2,
    samples: 2,
},
},
}
});
}

    async function getData() {
    const xs = [];
    const ys = [];
    const response = await fetch('dataDoma.csv');
    const data = await response.text();
//console.log(data);
    const table = data.split(/\n/).slice(1);
    table.forEach(row => {
    const columns = row.split(',');
    const time = columns[0];
    datumydoma.push(time);
    xs.push(time);
    const temp = columns[1];
    tepotadoma.push(temp);
    const vlh = columns[2];
    vlhkostdoma.push(vlh);
    ys.push(parseFloat(temp) + 14);

    const firstFruits = [];

    for (let i = 0; i < vlhkostdoma.length; i = i+1) {
    firstFruits.push(vlhkostdoma[i]);
}
    for (let j = 0; j < tepotadoma.length; j = j+1) {
    firstFruits.push(tepotadoma[j]);
}
    for (let l = 0; l < datumydoma.length; l = l+1) {
    firstFruits.push(datumydoma[l]);
}
})
}
</script>