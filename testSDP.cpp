

void test_addOneByOne()
{
	cout << "Start test addOneByOne"<<endl ;
	Market m(2);
	Client c{ -1, 40, 0 };

	for (int i = 0; i < 10; i++)
	{
		m.AddClient(&c, 1);
	}

	MarketState state = m.getMarketState();

	if (state.numberOfCashDesk != 2)
	{
		cout << "we expected 2 open cash desk but we found : " << state.numberOfCashDesk << endl;
		return;
	}
	if (state.numberOfClientsAtCashDecsk[0] != 5)
	{
		cout << "we expected 5 clients on cash desk [0] but found :" << state.numberOfClientsAtCashDecsk[0] << endl;
		return;
	}
	if (state.numberOfClientsAtCashDecsk[1] != 5)
	{
		cout << "we expected 5 clients on cash desk [1] but found :" << state.numberOfClientsAtCashDecsk[1] << endl;
		return;
	}
	if (state.numberOfClientsAtExpressCashDeck != 0)
	{
		cout << "we expected 0 client on expres cash desk but found :" << state.numberOfClientsAtExpressCashDeck << endl;
		return;
	}
	cout << "test addOneByOne is OK"<<endl;
}

void test_expresCashDesk()
{

	cout << "Start test expresCashDesk" << endl;
	Market m(1);
	Client poor{ -1, 2, 0 }, rich{ -1, 10, 0 };

	for (int i = 0; i < 5; i++)
	{
		m.AddClient(&poor, 1);
		m.AddClient(&rich, 1);
	}

	MarketState state = m.getMarketState();

	if (state.numberOfClientsAtCashDecsk[0] != 6)
	{
		cout << "we expected 6 clients on cash desk [0] but found :" << state.numberOfClientsAtCashDecsk[0] << endl;
		return;
	}
	if (state.numberOfClientsAtExpressCashDeck != 2)
	{
		cout << "we expected 2 client on expres cash desk but found :" << state.numberOfClientsAtExpressCashDeck << endl;
		return;
	}
	cout << "test expresCashDesk is OK" << endl;
}

void test_manyClientsInOneTick()
{
	cout << "Start test manyClientsInOneTick" << endl;
	Market m(4);
	Client crowd[10];

	for (int i = 0; i < 10; i++)
	{
		crowd[i].creditCard = 0;
		crowd[i].ID = -1;
		crowd[i].numberOfGoods = i;
	}

	m.AddClient(crowd, 10);

	MarketState state = m.getMarketState();

	if (state.numberOfCashDesk != 2)
	{
		cout << "we expected 2 open cash desk but we found : " << state.numberOfCashDesk << endl;
		return;
	}
	if (state.numberOfClientsAtCashDecsk[0] != 3)
	{
		cout << "we expected 5 clients on cash desk [0] but found :" << state.numberOfClientsAtCashDecsk[0] << endl;
		return;
	}
	if (state.numberOfClientsAtCashDecsk[1] != 3)
	{
		cout << "we expected 5 clients on cash desk [1] but found :" << state.numberOfClientsAtCashDecsk[1] << endl;
		return;
	}
	if (state.numberOfClientsAtExpressCashDeck != 3)
	{
		cout << "we expected 0 client on expres cash desk but found :" << state.numberOfClientsAtExpressCashDeck << endl;
		return;
	}

	cout << "test manyClientsInOneTick is OK"<< endl;
}

void test_unloadMarket()
{
	cout << "Start test unloadMarket" << endl;
	Market m(4);
	Client crowd[10];

	for (int i = 0; i < 10; i++)
	{
		crowd[i].creditCard = 0;
		crowd[i].ID = -1;
		crowd[i].numberOfGoods = i;
	}

	m.AddClient(crowd, 10);

	for (int i = 0; i < 25; i++)
	{
		m.AddClient(NULL, 0);
	}

	MarketState state = m.getMarketState();

	if (state.numberOfCashDesk != 1)
	{
		cout << "we expected 1 open cash desk but we found : " << state.numberOfCashDesk << endl;
		return;
	}
	if (state.numberOfClientsAtCashDecsk[0] != 1)
	{
		cout << "we expected 1 client on cash desk [0] but found :" << state.numberOfClientsAtCashDecsk[0] << endl;
		return;
	}
	if (state.numberOfClientsAtExpressCashDeck != 0)
	{
		cout << "we expected 0 client on expres cash desk but found :" << state.numberOfClientsAtExpressCashDeck << endl;
		return;
	}
	cout << "test unloadMarket is OK"<< endl;
}

void test_creditCard()
{
	cout << "Start test creditCard" << endl;
	Market m(1);
	Client crowd[10];

	for (int i = 0; i < 10; i++)
	{
		crowd[i].creditCard = 1;
		crowd[i].ID = -1;
		crowd[i].numberOfGoods = 1;
	}

	m.AddClient(crowd, 10);

	for (int i = 0; i < 5; i++)
	{
		m.AddClient(NULL, 0);
	}

	MarketState state = m.getMarketState();

	if (state.numberOfClientsAtCashDecsk[0] != 6)
	{
		cout << "we expected 6 client on cash desk [0] but found :" << state.numberOfClientsAtCashDecsk[0] << endl;
		return;
	}
	if (state.numberOfClientsAtExpressCashDeck != 0)
	{
		cout << "we expected 0 client on expres cash desk but found :" << state.numberOfClientsAtExpressCashDeck << endl;
		return;
	}
	cout << "test creditCard is OK" << endl;
}



int main()
{
	test_addOneByOne();
	test_creditCard();
	test_expresCashDesk();
	test_manyClientsInOneTick();
	test_unloadMarket();
	cin.get();
}