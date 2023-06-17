const ssid = document.querySelector("#ssidInput");
const password = document.querySelector("#passwordInput");
const loading = document.querySelector(".loooo");
const ip = document.querySelector(".ipppp");

let first = true;

const toogleLoading = () => {
	loading.classList.toggle("hidden");
};

const toogleResult = () => {
	ip.classList.toggle("hidden");
};

const getData = async () => {
	const endpoint = `http://192.168.4.1/update?ssid=${ssid.value}&password=${password.value}`;
	console.log(endpoint);
	const res = await fetch(endpoint)
                    .then(res => res.text());

	if (res === "ssid or password is not valid" || res === "0.0.0.0") return getData();

	return res;
};

const sendData = async (e) => {
	e.preventDefault();
	toogleLoading();
	if (!first) {
		toogleResult();
	}

	const result = await getData();

	ip.innerHTML = result;

    if(result) {
        toogleLoading();
        toogleResult();
    }

	first = false;
};
