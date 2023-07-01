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
	const endpoint = `/be-update?ipBackend=${ssid.value}&apiKey=${password.value}`;
	// alert(endpoint);
	const res = await fetch(endpoint)
                    .then(res => res.text());

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
