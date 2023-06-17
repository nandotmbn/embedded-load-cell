const months = [
	"Januari",
	"Februari",
	"Maret",
	"April",
	"Mei",
	"Juni",
	"Juli",
	"Agustus",
	"September",
	"Oktober",
	"November",
	"Desember",
];

let stringLog = "";

setInterval(() => {
	fetch("/get-data")
		.then((response) => response.text())
		.then((data) => {
			document.getElementsByClassName("data1")[0].innerHTML = parseFloat(
				data.split(",")[0]
			).toFixed(2);
			document.getElementsByClassName("data2")[0].innerHTML = parseFloat(
				data.split(",")[1]
			).toFixed(2);
			document.getElementsByClassName("data3")[0].innerHTML = parseFloat(
				data.split(",")[2]
			).toFixed(2);
			document.getElementsByClassName("data4")[0].innerHTML = parseFloat(
				data.split(",")[3]
			).toFixed(2);
			stringLog += `${parseFloat(data.split(",")[0]).toFixed(2)}, ${parseFloat(
				data.split(",")[1]
			).toFixed(2)}, ${parseFloat(data.split(",")[2]).toFixed(2)}, ${parseFloat(
				data.split(",")[3]
			).toFixed(2)} \r\n`;
			document.getElementById("chat").innerHTML = stringLog;
		});
}, 2000);

document.getElementById("clear-button").addEventListener("click", (e) => {
	stringLog = "";
	document.getElementById("chat").innerHTML = ""
});

setInterval(() => {
	const now = new Date();
	const date = now.getDate();
	const month = now.getMonth();
	const year = now.getFullYear();

	const hour = now.getHours();
	const minute = now.getMinutes();
	const second = now.getSeconds();
	document.getElementsByClassName(
		"date"
	)[0].innerHTML = `${date} ${months[month]} ${year}`;

	document.getElementsByClassName(
		"time"
	)[0].innerHTML = `${hour}:${minute}:${second}`;
}, 500);
